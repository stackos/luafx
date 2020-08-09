#import <AppKit/NSViewController.h>
#import <AppKit/NSWindow.h>

@interface ViewController : NSViewController

- (void)setWindow:(NSWindow*)window;
- (void)initRenderer;
- (void)onResize:(int)width :(int)height;

@end
