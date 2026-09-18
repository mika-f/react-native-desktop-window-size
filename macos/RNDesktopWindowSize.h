#import <React/RCTBridgeModule.h>
#ifdef RCT_NEW_ARCH_ENABLED
#import <RNDesktopWindowSizeSpec/RNDesktopWindowSizeSpec.h>
@interface RNDesktopWindowSize : NSObject <NativeDesktopWindowSizeSpec>
#else
@interface RNDesktopWindowSize : NSObject <RCTBridgeModule>
#endif
@end
