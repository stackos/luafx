#pragma once

#if defined(LFX_WINDOWS)
#include "GL/glew.h"
#elif defined(LFX_ANDROID)
#include "platform/android/gl3stub.h"
#elif defined(LFX_MAC)
#include "GL/glew.h"
#elif defined(LFX_IOS)
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
not implement.
#endif
