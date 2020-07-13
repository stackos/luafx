#pragma once

#include "luafx.h"

// class
typedef struct LFX_ObjectPool
{
    void** object_ptr_array;
    int array_size;
    int object_count;
    int object_size;
} LFX_ObjectPool;

// public
void LFX_ObjectPool_Init(LFX_ObjectPool* thiz, int object_size);
void LFX_ObjectPool_Done(LFX_ObjectPool* thiz);
void LFX_ObjectPool_Grow(LFX_ObjectPool* thiz);
void LFX_ObjectPool_Release(LFX_ObjectPool* thiz);
int LFX_ObjectPool_CreateObject(LFX_ObjectPool* thiz);
void* LFX_ObjectPool_GetObject(LFX_ObjectPool* thiz, int id);
void LFX_ObjectPool_DestroyObject(LFX_ObjectPool* thiz, int id);
