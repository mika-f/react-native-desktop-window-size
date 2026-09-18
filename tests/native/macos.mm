#import "RNDWSWindowOperations.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

static void Check(BOOL condition, const char *message) {
  if (!condition) { fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static NSWindow *Window() {
  return [[NSWindow alloc] initWithContentRect:NSMakeRect(100, 100, 800, 600)
      styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable
      backing:NSBackingStoreBuffered defer:NO];
}
static BOOL SizeIs(NSWindow *window, CGFloat width, CGFloat height) {
  NSSize size = [RNDWSWindowOperations sizeOfWindow:window];
  return std::abs(size.width - width) < 1 && std::abs(size.height - height) < 1;
}
static void WaitFor(BOOL (^condition)(void), const char *message) {
  NSDate *deadline = [NSDate dateWithTimeIntervalSinceNow:5];
  while (!condition() && deadline.timeIntervalSinceNow > 0) {
    [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.02]];
  }
  Check(condition(), message);
}
int main() {
  @autoreleasepool {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp finishLaunching];
    NSWindow *first = Window();
    NSWindow *second = Window();
    Check(![RNDWSWindowOperations setSize:NSMakeSize(900, 700) forWindow:first], "resize succeeds");
    Check(SizeIs(first, 900, 700), "content size round trip");
    Check(SizeIs(second, 800, 600), "another window is unchanged");
    NSSize originalMinimum = first.contentMinSize;
    NSSize originalMaximum = first.contentMaxSize;
    Check(![RNDWSWindowOperations setMinimum:[NSValue valueWithSize:NSMakeSize(500, 400)] forWindow:first], "set minimum");
    Check(![RNDWSWindowOperations setMaximum:[NSValue valueWithSize:NSMakeSize(850, 650)] forWindow:first], "set maximum");
    Check(SizeIs(first, 850, 650), "new maximum clamps current size");
    [RNDWSWindowOperations setSize:NSMakeSize(100, 100) forWindow:first];
    Check(SizeIs(first, 500, 400), "resize clamps to minimum");
    NSError *conflict = [RNDWSWindowOperations setMinimum:[NSValue valueWithSize:NSMakeSize(900, 400)] forWindow:first];
    Check([conflict.domain isEqualToString:@"E_INVALID_CONSTRAINTS"], "conflicting limits reject");
    Check(first.contentMinSize.width == 500, "rejected limit leaves existing limit intact");
    Check([RNDWSWindowOperations setSize:NSMakeSize(NAN, 600) forWindow:first] != nil, "NaN rejects");
    Check([RNDWSWindowOperations setSize:NSMakeSize(-1, 600) forWindow:first] != nil, "negative size rejects");
    Check(![RNDWSWindowOperations setMinimum:nil forWindow:first], "reset minimum");
    Check(![RNDWSWindowOperations setMaximum:nil forWindow:first], "reset maximum");
    Check(NSEqualSizes(first.contentMinSize, originalMinimum), "original minimum restored");
    Check(NSEqualSizes(first.contentMaxSize, originalMaximum), "original maximum restored");
    [RNDWSWindowOperations setSize:NSMakeSize(800, 600) forWindow:first];
    Check(![RNDWSWindowOperations maximize:first], "maximize succeeds");
    NSRect maximized = first.frame;
    Check(![RNDWSWindowOperations maximize:first], "repeated maximize succeeds");
    Check(NSEqualRects(first.frame, maximized), "maximize is idempotent");
    Check(![RNDWSWindowOperations restore:first], "restore succeeds");
    Check(SizeIs(first, 800, 600), "restore returns to normal size");
    [first orderFront:nil];
    Check(![RNDWSWindowOperations minimize:first], "minimize succeeds");
    WaitFor(^BOOL { return first.miniaturized; }, "window is minimized");
    Check(![RNDWSWindowOperations restore:first], "restore from minimized succeeds");
    WaitFor(^BOOL { return !first.miniaturized; }, "window is restored");
    first.releasedWhenClosed = NO;
    second.releasedWhenClosed = NO;
    [first close]; [second close];
    puts("AppKit window tests passed.");
  }
}
