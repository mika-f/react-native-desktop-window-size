#import <AppKit/AppKit.h>

NS_ASSUME_NONNULL_BEGIN

/** UI-thread-only AppKit operations, kept separate from React Native for testing. */
@interface RNDWSWindowOperations : NSObject
+ (NSSize)sizeOfWindow:(NSWindow *)window;
+ (nullable NSError *)setSize:(NSSize)size forWindow:(NSWindow *)window;
+ (nullable NSError *)setMinimum:(nullable NSValue *)size forWindow:(NSWindow *)window;
+ (nullable NSError *)setMaximum:(nullable NSValue *)size forWindow:(NSWindow *)window;
+ (nullable NSError *)maximize:(NSWindow *)window;
+ (nullable NSError *)minimize:(NSWindow *)window;
+ (nullable NSError *)restore:(NSWindow *)window;
@end

NS_ASSUME_NONNULL_END
