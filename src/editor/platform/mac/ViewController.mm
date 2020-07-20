#import "ViewController.h"
#include "luafx.h"
#include "gl_include.h"
#import <AppKit/AppKit.h>

// ViewController
 @interface ViewController ()

 @property(nonatomic, weak) NSWindow* window;
 @property(nonatomic, weak) NSOpenGLContext* gl_context;

- (void)drawFrame;

 @end

// FrameHandler
@interface FrameHandler : NSObject

@property(nonatomic, weak) ViewController* vc;

@end

@implementation FrameHandler

- (void)drawFrame {
    [_vc drawFrame];
}

@end

// ViewController
@implementation ViewController {
    int m_target_width;
    int m_target_height;
    FrameHandler* m_frame_handler;
    NSTimer* m_timer;
    int m_context;
    int m_effect;
    LFX_Texture m_texture_in;
    LFX_Texture m_texture_out;
    void* m_image_in;
    void* m_image_out;
}

- (void)setWindow:(NSWindow*)window {
    _window = window;
}

- (void)loadView {
    CGSize size = [_window contentRectForFrameRect:_window.contentLayoutRect].size;
    NSRect rect = NSMakeRect(0, 0, size.width, size.height);
    NSOpenGLPixelFormatAttribute attributes[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        0
    };
    NSOpenGLPixelFormat* format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    NSOpenGLView* view = [[NSOpenGLView alloc] initWithFrame:rect pixelFormat:format];
    view.wantsBestResolutionOpenGLSurface = TRUE;
    [view prepareOpenGL];
    _gl_context = view.openGLContext;
    self.view = view;
    
    [_gl_context makeCurrentContext];
    glewInit();
    
    float scale = _window.backingScaleFactor;
    m_target_width = size.width * scale;
    m_target_height = size.height * scale;
    
    m_frame_handler = [FrameHandler new];
    m_frame_handler.vc = self;
    m_timer = [NSTimer timerWithTimeInterval:1.0f / 60 target:m_frame_handler selector:@selector(drawFrame) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:m_timer forMode:NSDefaultRunLoopMode];
}

- (void)initRenderer {
    [self initContext];
}

- (void)dealloc {
    [self doneContext];
    
    [m_timer invalidate];
}

- (void)onResize:(int)width :(int)height {
    m_target_width = width;
    m_target_height = height;
}

- (void)drawFrame {
    LFX_RenderEffect(m_context, m_effect, &m_texture_in, &m_texture_out, m_image_out);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_target_width, m_target_height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    int w = m_target_width;
    int h = m_target_height;
    glViewport(0, 0, w, h);
    LFX_RenderQuad(m_context, &m_texture_out, NULL);
    
    [_gl_context flushBuffer];
}

- (void)initContext {
    const char* res_dir = [[[NSBundle mainBundle] resourcePath] UTF8String];
    
    LFX_CreateContext(&m_context);
    
    LFX_Path path;
    sprintf(path, "%s/%s", res_dir, "assets/effect.lua");
    LFX_LoadEffect(m_context, path, &m_effect);
    
    sprintf(path, "%s/%s", res_dir, "assets/input/1080x1920.jpg");
    LFX_LoadTexture2D(m_context, path, &m_texture_in);
    
    m_image_in = malloc(m_texture_in.width * m_texture_in.height * 4);
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture_in.target, m_texture_in.id, 0);
    glReadPixels(0, 0, m_texture_in.width, m_texture_in.height, m_texture_in.format, GL_UNSIGNED_BYTE, m_image_in);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    
    memset(&m_texture_out, 0, sizeof(m_texture_out));
    m_texture_out.target = GL_TEXTURE_2D;
    m_texture_out.format = GL_RGBA;
    m_texture_out.width = m_texture_in.width;
    m_texture_out.height = m_texture_in.height;
    m_texture_out.filter_mode = GL_LINEAR;
    m_texture_out.wrap_mode = GL_CLAMP_TO_EDGE;
    LFX_CreateTexture(m_context, &m_texture_out);
    
    m_image_out = malloc(m_texture_out.width * m_texture_out.height * 4);
    
    [self resizeWindow:m_texture_in.width :m_texture_in.height];
}

- (void)doneContext {
    free(m_image_in);
    free(m_image_out);
    LFX_DestroyTexture(m_context, &m_texture_in);
    LFX_DestroyTexture(m_context, &m_texture_out);
    LFX_DestroyEffect(m_context, m_effect);
    LFX_DestroyContext(m_context);
}

- (void)resizeWindow:(int)input_width :(int)input_height {
    NSScreen* screen = NSScreen.mainScreen;
    NSRect screen_rect = [screen convertRectToBacking:screen.visibleFrame];
    
    const int MAX_W = screen_rect.size.width;
    const int MAX_H = screen_rect.size.height;
    int w = input_width;
    int h = input_height;
    float scale_w = 1.0f;
    float scale_h = 1.0f;
    if (w > MAX_W)
    {
        scale_w = MAX_W / (float) w;
    }
    if (h > MAX_H)
    {
        scale_h = MAX_H / (float) h;
    }
    float scale = fmin(scale_w, scale_h);
    w = (int) (w * scale);
    h = (int) (h * scale);
    
    m_target_width = w;
    m_target_height = h;
    
    float window_scale = _window.backingScaleFactor;
    w /= window_scale;
    h /= window_scale;
    
    NSRect rect = NSMakeRect(0, 0, w, h);
    NSRect frame = [_window frameRectForContentRect:rect];
    
    [_window setFrame:frame display:TRUE animate:FALSE];
    [_window center];
}

@end
