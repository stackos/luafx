#include "luafx.h"
#include <emscripten.h>

void EMSCRIPTEN_KEEPALIVE InitEngine(const char* msg)
{
    LFX_CreateContext(NULL);
}

void EMSCRIPTEN_KEEPALIVE DoneEngine(const char* msg)
{
    LFX_DestroyContext(0);
}

void EMSCRIPTEN_KEEPALIVE UpdateEngine(const char* msg)
{
    LFX_RenderEffect(0, 0, NULL, NULL, NULL);
}

int main()
{
    return 0;
}
