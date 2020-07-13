#import "AppDelegate.h"
#import "ViewController.h"

@implementation AppDelegate {
    UIWindow* m_window;
}

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    m_window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    m_window.rootViewController = [ViewController new];
    [m_window makeKeyAndVisible];
    
    return YES;
}

@end
