#import "RNDesktopWindowSize.h"
#import "RNDWSWindowOperations.h"

@implementation RNDesktopWindowSize
RCT_EXPORT_MODULE(DesktopWindowSize)
@synthesize viewRegistry_DEPRECATED = _viewRegistry_DEPRECATED;

+ (BOOL)requiresMainQueueSetup { return NO; }

- (void)withWindow:(double)tag
          resolve:(RCTPromiseResolveBlock)resolve
           reject:(RCTPromiseRejectBlock)reject
           action:(id (^)(NSWindow *, NSError **))action
{
  // Resolve through the caller's React view registry, never NSApp.keyWindow.
  dispatch_async(dispatch_get_main_queue(), ^{
    NSWindow *window = [self.viewRegistry_DEPRECATED viewForReactTag:@(tag)].window;
    if (!window) {
      reject(@"E_NO_WINDOW", @"The target view is not mounted in a window. Use a native View with collapsable={false} after onLayout.", nil);
      return;
    }
    NSError *error = nil;
    id result = action(window, &error);
    if (error) reject(error.domain, error.localizedDescription, error);
    else resolve(result);
  });
}

RCT_EXPORT_METHOD(getSize:(double)tag resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject)
{
  [self withWindow:tag resolve:resolve reject:reject action:^id(NSWindow *window, NSError **error) {
    NSSize size = [RNDWSWindowOperations sizeOfWindow:window];
    return @{ @"width": @(size.width), @"height": @(size.height) };
  }];
}

RCT_EXPORT_METHOD(setSize:(double)tag width:(double)width height:(double)height
                  resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject)
{
  [self withWindow:tag resolve:resolve reject:reject action:^id(NSWindow *window, NSError **error) {
    *error = [RNDWSWindowOperations setSize:NSMakeSize(width, height) forWindow:window];
    return nil;
  }];
}

RCT_EXPORT_METHOD(setMinSize:(double)tag width:(NSNumber *)width height:(NSNumber *)height
                  resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject)
{
  [self withWindow:tag resolve:resolve reject:reject action:^id(NSWindow *window, NSError **error) {
    if ((width == nil) != (height == nil)) {
      *error = [NSError errorWithDomain:@"E_INVALID_SIZE" code:1 userInfo:@{NSLocalizedDescriptionKey: @"Both dimensions must be specified or both must be null."}];
      return nil;
    }
    NSValue *size = width && height ? [NSValue valueWithSize:NSMakeSize(width.doubleValue, height.doubleValue)] : nil;
    *error = [RNDWSWindowOperations setMinimum:size forWindow:window];
    return nil;
  }];
}

RCT_EXPORT_METHOD(setMaxSize:(double)tag width:(NSNumber *)width height:(NSNumber *)height
                  resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject)
{
  [self withWindow:tag resolve:resolve reject:reject action:^id(NSWindow *window, NSError **error) {
    if ((width == nil) != (height == nil)) {
      *error = [NSError errorWithDomain:@"E_INVALID_SIZE" code:1 userInfo:@{NSLocalizedDescriptionKey: @"Both dimensions must be specified or both must be null."}];
      return nil;
    }
    NSValue *size = width && height ? [NSValue valueWithSize:NSMakeSize(width.doubleValue, height.doubleValue)] : nil;
    *error = [RNDWSWindowOperations setMaximum:size forWindow:window];
    return nil;
  }];
}

RCT_EXPORT_METHOD(maximize:(double)tag resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject)
{
  [self withWindow:tag resolve:resolve reject:reject action:^id(NSWindow *window, NSError **error) {
    *error = [RNDWSWindowOperations maximize:window]; return nil;
  }];
}

RCT_EXPORT_METHOD(minimize:(double)tag resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject)
{
  [self withWindow:tag resolve:resolve reject:reject action:^id(NSWindow *window, NSError **error) {
    *error = [RNDWSWindowOperations minimize:window]; return nil;
  }];
}

RCT_EXPORT_METHOD(restore:(double)tag resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject)
{
  [self withWindow:tag resolve:resolve reject:reject action:^id(NSWindow *window, NSError **error) {
    *error = [RNDWSWindowOperations restore:window]; return nil;
  }];
}

#ifdef RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params
{
  return std::make_shared<facebook::react::NativeDesktopWindowSizeSpecJSI>(params);
}
#endif
@end
