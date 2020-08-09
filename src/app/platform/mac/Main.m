#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char* argv[]) {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        AppDelegate* del = [AppDelegate new];
        app.delegate = del;
        [app run];
    }
    return 0;
}
