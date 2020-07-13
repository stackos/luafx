#import "AppDelegate.h"
#import "ViewController.h"

@implementation AppDelegate {
    NSWindow* m_window;
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
    const char* name = "oflite_editor";
    NSRect rect = NSMakeRect(0, 0, 1280, 720);
    int style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
    NSRect frame = [NSWindow frameRectForContentRect:rect styleMask:style];
    
    NSWindow* window = [[NSWindow alloc] initWithContentRect:frame styleMask:style backing:NSBackingStoreBuffered defer:TRUE];
    window.title = [NSString stringWithUTF8String:name];
    [window makeKeyAndOrderFront:nil];
    window.delegate = self;
    [window center];
    
    ViewController* vc = [[ViewController alloc] init];
    [vc setWindow:window];
    window.contentViewController = vc;
    [vc initRenderer];
    
    m_window = window;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return TRUE;
}

- (NSSize)windowWillResize:(NSWindow*)window toSize:(NSSize)frameSize {
    CGSize size = [window contentRectForFrameRect:NSMakeRect(0, 0, frameSize.width, frameSize.height)].size;
    float scale = window.backingScaleFactor;
    int width = size.width * scale;
    int height = size.height * scale;
    [(ViewController*) window.contentViewController onResize:width :height];
    return frameSize;
}

@end
