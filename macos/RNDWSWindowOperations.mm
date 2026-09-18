#import "RNDWSWindowOperations.h"
#import <objc/runtime.h>
#include <cmath>

@interface RNDWSWindowLimits : NSObject
@property NSSize originalMinimum;
@property NSSize originalMaximum;
@end
@implementation RNDWSWindowLimits
@end

static const void *LimitsKey = &LimitsKey;

static NSError *Failure(NSString *code, NSString *message)
{
  return [NSError errorWithDomain:code code:1 userInfo:@{NSLocalizedDescriptionKey: message}];
}

static BOOL ValidSize(NSSize size)
{
  return std::isfinite(size.width) && std::isfinite(size.height) &&
      size.width > 0 && size.height > 0 && size.width <= 1000000 && size.height <= 1000000;
}

static RNDWSWindowLimits *Limits(NSWindow *window)
{
  RNDWSWindowLimits *limits = objc_getAssociatedObject(window, LimitsKey);
  if (!limits) {
    limits = [RNDWSWindowLimits new];
    limits.originalMinimum = window.contentMinSize;
    limits.originalMaximum = window.contentMaxSize;
    objc_setAssociatedObject(window, LimitsKey, limits, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  }
  return limits;
}

static NSSize Clamped(NSSize size, NSWindow *window)
{
  NSSize minimum = window.contentMinSize;
  NSSize maximum = window.contentMaxSize;
  return NSMakeSize(MIN(MAX(size.width, minimum.width), maximum.width),
                    MIN(MAX(size.height, minimum.height), maximum.height));
}

static NSError *CheckWindow(NSWindow *window)
{
  if (window.styleMask & NSWindowStyleMaskFullScreen) {
    return Failure(@"E_UNSUPPORTED_STATE", @"Exit full screen before changing window size or state.");
  }
  return nil;
}

@implementation RNDWSWindowOperations

+ (NSSize)sizeOfWindow:(NSWindow *)window
{
  return [window contentRectForFrameRect:window.frame].size;
}

+ (NSError *)restore:(NSWindow *)window
{
  if (NSError *error = CheckWindow(window)) return error;
  if (window.miniaturized) [window deminiaturize:nil];
  if (window.zoomed) [window zoom:nil];
  return nil;
}

+ (NSError *)setSize:(NSSize)size forWindow:(NSWindow *)window
{
  if (!ValidSize(size)) return Failure(@"E_INVALID_SIZE", @"Window dimensions must be positive, finite, and at most 1,000,000.");
  if (NSError *error = [self restore:window]) return error;
  [window setContentSize:Clamped(size, window)];
  return nil;
}

+ (NSError *)setMinimum:(NSValue *)value forWindow:(NSWindow *)window
{
  if (value && !ValidSize(value.sizeValue)) return Failure(@"E_INVALID_SIZE", @"Invalid minimum size.");
  if (NSError *error = CheckWindow(window)) return error;
  NSSize size = value ? value.sizeValue : Limits(window).originalMinimum;
  NSSize maximum = window.contentMaxSize;
  if (size.width > maximum.width || size.height > maximum.height) {
    return Failure(@"E_INVALID_CONSTRAINTS", @"Minimum size cannot exceed maximum size.");
  }
  if (NSError *error = [self restore:window]) return error;
  Limits(window);
  window.contentMinSize = size;
  [window setContentSize:Clamped([self sizeOfWindow:window], window)];
  return nil;
}

+ (NSError *)setMaximum:(NSValue *)value forWindow:(NSWindow *)window
{
  if (value && !ValidSize(value.sizeValue)) return Failure(@"E_INVALID_SIZE", @"Invalid maximum size.");
  if (NSError *error = CheckWindow(window)) return error;
  NSSize size = value ? value.sizeValue : Limits(window).originalMaximum;
  NSSize minimum = window.contentMinSize;
  if (size.width < minimum.width || size.height < minimum.height) {
    return Failure(@"E_INVALID_CONSTRAINTS", @"Maximum size cannot be smaller than minimum size.");
  }
  if (NSError *error = [self restore:window]) return error;
  Limits(window);
  window.contentMaxSize = size;
  [window setContentSize:Clamped([self sizeOfWindow:window], window)];
  return nil;
}

+ (NSError *)maximize:(NSWindow *)window
{
  if (NSError *error = CheckWindow(window)) return error;
  if (!(window.styleMask & NSWindowStyleMaskResizable)) {
    return Failure(@"E_UNSUPPORTED_STATE", @"The window is not resizable.");
  }
  if (window.miniaturized) [window deminiaturize:nil];
  if (!window.zoomed) [window zoom:nil];
  return nil;
}

+ (NSError *)minimize:(NSWindow *)window
{
  if (NSError *error = CheckWindow(window)) return error;
  if (!(window.styleMask & NSWindowStyleMaskMiniaturizable)) {
    return Failure(@"E_UNSUPPORTED_STATE", @"The window cannot be minimized.");
  }
  if (!window.miniaturized) [window miniaturize:nil];
  return nil;
}
@end
