#import "ViewController.h"
#include "gl_include.h"
#include "oflite.h"
#include <assert.h>

// View
@interface View : UIView
@end

@implementation View

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (instancetype)init {
    if (self = [super initWithFrame:UIScreen.mainScreen.bounds]) {
        self.contentScaleFactor = UIScreen.mainScreen.nativeScale;
        CAEAGLLayer* layer = (CAEAGLLayer*) self.layer;
        layer.opaque = YES;
    }
    return self;
}

@end

// ViewController
@interface ViewController ()

- (void)drawFrame;

@end

// FrameHandler
@interface FrameHandler : NSObject

@property(weak) ViewController* vc;

@end

@implementation FrameHandler

- (void)drawFrame {
    [_vc drawFrame];
}

@end

// ViewController
@implementation ViewController {
    EAGLContext* m_gl_context;
    GLuint m_frame_buffer;
    GLuint m_render_buffer;
    int m_target_width;
    int m_target_height;
    FrameHandler* m_frame_handler;
    CADisplayLink* m_display_link;
    UIDeviceOrientation m_orientation;
    int m_ofl_context;
    int m_ofl_effect;
    OFL_Texture m_texture_in;
    OFL_Texture m_texture_out;
    void* m_image_in;
    void* m_image_out;
}

- (void)loadView {
    self.view = [View new];
    
    [self initGL];
    [self initOFLContext];
    
    m_frame_handler = [FrameHandler new];
    m_frame_handler.vc = self;
    m_display_link = [CADisplayLink displayLinkWithTarget:m_frame_handler selector:@selector(drawFrame)];
    [m_display_link addToRunLoop:NSRunLoop.currentRunLoop forMode:NSDefaultRunLoopMode];
    
    m_orientation = UIDevice.currentDevice.orientation;
    [NSNotificationCenter.defaultCenter addObserver:self selector:@selector(orientationDidChange:) name:UIDeviceOrientationDidChangeNotification object:nil];
}

- (void)initGL {
    EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (context == nil) {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    }
    if (context == nil) {
        return;
    }
    m_gl_context = context;
    
    [EAGLContext setCurrentContext:m_gl_context];
    
    glGenFramebuffers(1, &m_frame_buffer);
    glGenRenderbuffers(1, &m_render_buffer);
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_render_buffer);
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*) self.view.layer];
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_render_buffer);
    
    GLint width;
    GLint height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    m_target_width = width;
    m_target_height = height;
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);
}

- (void)dealloc {
    [NSNotificationCenter.defaultCenter removeObserver:self];
    
    [self doneOFLContext];
    
    glDeleteFramebuffers(1, &m_frame_buffer);
    glDeleteRenderbuffers(1, &m_render_buffer);
    [EAGLContext setCurrentContext:nil];
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)orientationDidChange:(NSNotification*)notification {
    UIDeviceOrientation orientation = UIDevice.currentDevice.orientation;
    m_orientation = orientation;
}

- (void)drawFrame {
    OFL_RenderEffect(m_ofl_context, m_ofl_effect, &m_texture_in, &m_texture_out, m_image_out);

    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
    glViewport(0, 0, m_target_width, m_target_height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    OFL_RenderQuad(m_ofl_context, &m_texture_out, NULL);
    
    glBindRenderbuffer(GL_RENDERBUFFER, m_render_buffer);
    [m_gl_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)initOFLContext {
    const char* res_dir = [[[NSBundle mainBundle] resourcePath] UTF8String];
    
    OFL_CreateContext(&m_ofl_context);
    
    OFL_Path path;
    sprintf(path, "%s/%s", res_dir, "assets/effect.ofeffect");
    OFL_LoadEffect(m_ofl_context, path, &m_ofl_effect);
    
    sprintf(path, "%s/%s", res_dir, "assets/1080x1920.jpg");
    OFL_LoadTexture2D(m_ofl_context, path, &m_texture_in);
    
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
    OFL_CreateTexture(m_ofl_context, &m_texture_out);
    
    m_image_out = malloc(m_texture_out.width * m_texture_out.height * 4);
}

- (void)doneOFLContext {
    free(m_image_in);
    free(m_image_out);
    OFL_DestroyTexture(m_ofl_context, &m_texture_in);
    OFL_DestroyTexture(m_ofl_context, &m_texture_out);
    OFL_DestroyEffect(m_ofl_context, m_ofl_effect);
    OFL_DestroyContext(m_ofl_context);
}

@end
