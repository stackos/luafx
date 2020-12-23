#include <Windows.h>
#include <windowsx.h>
#include "gl_include.h"
#include "luafx.h"

typedef struct GLContext
{
    HWND hwnd;
    HDC hdc;
    HGLRC context;
    int width;
    int height;
    int minimized;
} GLContext;

static void GLContext_Init(GLContext* thiz)
{
    thiz->hwnd = NULL;
    thiz->hdc = NULL;
    thiz->context = NULL;
    thiz->width = 0;
    thiz->height = 0;
    thiz->minimized = 0;
}

static void GLContext_Done(GLContext* thiz)
{
    if (thiz->context)
    {
        wglDeleteContext(thiz->context);
        thiz->context = NULL;
    }
    if (thiz->hdc)
    {
        ReleaseDC(thiz->hwnd, thiz->hdc);
        thiz->hdc = NULL;
    }
    thiz->hwnd = NULL;
}

static GLContext g_gl_context;
static int g_context = LFX_INVALID_HANDLE;
static int g_effect = LFX_INVALID_HANDLE;
static LFX_Texture g_texture_out;
static char* g_message_buffer = NULL;
static size_t g_message_buffer_size = 0;
static DWORD g_fps_update_time = 0;
static int g_frame_count = 0;
static DWORD g_start_time = 0;

static void InitGLContext(HWND hwnd, int width, int height)
{
    GLContext_Init(&g_gl_context);
    g_gl_context.hwnd = hwnd;
    g_gl_context.hdc = GetDC(hwnd);
    g_gl_context.width = width;
    g_gl_context.height = height;

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
        PFD_TYPE_RGBA,          // The kind of framebuffer. RGBA or palette.
        32,                     // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        0,                      // Number of bits for the depthbuffer
        0,                      // Number of bits for the stencilbuffer
        0,                      // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int pixel_format = ChoosePixelFormat(g_gl_context.hdc, &pfd);
    SetPixelFormat(g_gl_context.hdc, pixel_format, &pfd);

    g_gl_context.context = wglCreateContext(g_gl_context.hdc);
    wglMakeCurrent(g_gl_context.hdc, g_gl_context.context);

    glewInit();
}

static void DoneGLContext()
{
    GLContext_Done(&g_gl_context);
}

static void ResizeWindow(int input_width, int input_height)
{
    const int MAX_W = GetSystemMetrics(SM_CXSCREEN);
    const int MAX_H = (GetSystemMetrics(SM_CYSCREEN) - 86);
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
    float scale = min(scale_w, scale_h);
    w = (int) (w * scale);
    h = (int) (h * scale);

    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD style_ex = 0;

    RECT wr = { 0, 0, w, h };
    AdjustWindowRect(&wr, style, FALSE);

    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2 + wr.left;
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2 + wr.top;
    w = wr.right - wr.left;
    h = wr.bottom - wr.top;

    SetWindowPos(g_gl_context.hwnd, HWND_TOP, x, y, w, h, 0);
}

static void InitEffectContext()
{
    LFX_Path work_dir;
    {
        GetModuleFileName(NULL, work_dir, LFX_MAX_PATH);
        size_t len = strrchr(work_dir, '\\') - work_dir;
        work_dir[len] = 0;
    }

    LFX_CreateContext(&g_context);

    LFX_Path path;
    sprintf(path, "%s/../../../%s", work_dir, "assets/effect.lua");
    LFX_LoadEffect(g_context, path, &g_effect);

    memset(&g_texture_out, 0, sizeof(g_texture_out));
    g_texture_out.target = GL_TEXTURE_2D;
    g_texture_out.format = GL_RGBA;
    g_texture_out.width = 1280;
    g_texture_out.height = 720;
    g_texture_out.filter_mode = GL_LINEAR;
    g_texture_out.wrap_mode = GL_CLAMP_TO_EDGE;
    LFX_CreateTexture(g_context, &g_texture_out);

    ResizeWindow(g_texture_out.width, g_texture_out.height);

    // message buffer
    g_message_buffer_size = 1024;
    g_message_buffer = (char*) malloc(g_message_buffer_size);

    g_start_time = timeGetTime();
}

static void DoneEffectContext()
{
    free(g_message_buffer);
    LFX_DestroyTexture(g_context, &g_texture_out);
    LFX_DestroyEffect(g_context, g_effect);
    LFX_DestroyContext(g_context);
}

static void Init(HWND hwnd, int width, int height)
{
    InitGLContext(hwnd, width, height);
    InitEffectContext();
}

static void Done()
{
    DoneEffectContext();
    DoneGLContext();
}

static void SendMessageSetEffectTimestamp()
{
    int64_t timestamp = timeGetTime() - g_start_time;
    sprintf(g_message_buffer, "{\"timestamp\": %lld}", timestamp);
    LFX_SendEffectMessage(g_context, g_effect, LFX_MESSAGE_ID_SET_EFFECT_TIMESTAMP, g_message_buffer, NULL, 0);
}

static void SendMessageMouseDown(int x, int y)
{
    sprintf(g_message_buffer, "{\"x\": %d, \"y\": %d}", x, y);
    LFX_SendEffectMessage(g_context, g_effect, LFX_MESSAGE_ID_MOUSE_DOWN, g_message_buffer, NULL, 0);
}

static void SendMessageMouseUp(int x, int y)
{
    sprintf(g_message_buffer, "{\"x\": %d, \"y\": %d}", x, y);
    LFX_SendEffectMessage(g_context, g_effect, LFX_MESSAGE_ID_MOUSE_UP, g_message_buffer, NULL, 0);
}

static void SendMessageMouseMove(int x, int y)
{
    sprintf(g_message_buffer, "{\"x\": %d, \"y\": %d}", x, y);
    LFX_SendEffectMessage(g_context, g_effect, LFX_MESSAGE_ID_MOUSE_MOVE, g_message_buffer, NULL, 0);
}

static bool DrawFrame()
{
    SendMessageSetEffectTimestamp();

    LFX_RenderEffect(g_context, g_effect, NULL, &g_texture_out, NULL);

    glViewport(0, 0, g_gl_context.width, g_gl_context.height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    LFX_RenderQuad(g_context, &g_texture_out, NULL);

    SwapBuffers(g_gl_context.hdc);

    return true;
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
            {
                g_gl_context.minimized = 1;
            }
            else
            {
                if (!g_gl_context.minimized)
                {
                    int width = lParam & 0xffff;
                    int height = (lParam & 0xffff0000) >> 16;

                    if (g_gl_context.width != width || g_gl_context.height != height)
                    {
                        g_gl_context.width = width;
                        g_gl_context.height = height;
                    }
                }

                g_gl_context.minimized = 0;
            }
            break;

        case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            SendMessageMouseDown(x, y);
            break;
        }

        case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            SendMessageMouseUp(x, y);
            break;
        }

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            SendMessageMouseMove(x, y);
            break;
        }

        case WM_CLOSE:
            Done();
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    AllocConsole();
    FILE* stream_in = NULL;
    FILE* stream_out = NULL;
    freopen_s(&stream_in, "CON", "r", stdin);
    freopen_s(&stream_out, "CON", "w", stdout);

    const char* name = "luafx_app";
    int window_width = 1280;
    int window_height = 720;

    WNDCLASSEX win_class;
    ZeroMemory(&win_class, sizeof(win_class));

    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WindowProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = hInstance;
    win_class.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = name;
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hIcon = NULL;
    win_class.hIconSm = NULL;

    if (!RegisterClassEx(&win_class))
    {
        return 0;
    }

    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD style_ex = 0;

    RECT wr = { 0, 0, window_width, window_height };
    AdjustWindowRect(&wr, style, FALSE);

    HWND hwnd = NULL;

    {
        int x = (GetSystemMetrics(SM_CXSCREEN) - window_width) / 2 + wr.left;
        int y = (GetSystemMetrics(SM_CYSCREEN) - window_height) / 2 + wr.top;
        int w = wr.right - wr.left;
        int h = wr.bottom - wr.top;

        hwnd = CreateWindowEx(
            style_ex,   // window ex style
            name,       // class name
            name,       // app name
            style,      // window style
            x, y,       // x, y
            w, h,       // w, h
            NULL,    // handle to parent
            NULL,    // handle to menu
            hInstance,  // hInstance
            NULL);   // no extra parameters
    }

    if (!hwnd)
    {
        return 0;
    }

    Init(hwnd, window_width, window_height);

    ShowWindow(hwnd, SW_SHOW);

    int exit = 0;
    MSG msg;

    while (1)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                exit = 1;
                break;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        if (exit)
        {
            break;
        }

        if (!g_gl_context.minimized)
        {
            bool draw = DrawFrame();

            if (draw)
            {
                DWORD t = timeGetTime();
                if (t - g_fps_update_time > 1000)
                {
                    int fps = g_frame_count;
                    g_frame_count = 0;
                    g_fps_update_time = t;

                    char title[1024];
                    sprintf(title, "luafx_app [w: %d h: %d] [fps: %d] ", g_texture_out.width, g_texture_out.height, fps);
                    SetWindowText(hwnd, title);
                }
                g_frame_count += 1;
            }
        }
    }

    if (stream_in)
    {
        fclose(stream_in);
    }
    if (stream_out)
    {
        fclose(stream_out);
    }
    FreeConsole();

    return 0;
}
