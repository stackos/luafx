#pragma once

#include "luafx.h"

// class
typedef struct LFX_Vector
{
    void* data;
    int capacity;
    int count;
    int sizeof_element;
} LFX_Vector;

void LFX_Vector_Init(LFX_Vector* thiz, int sizeof_element);
void LFX_Vector_Done(LFX_Vector* thiz);
void LFX_Vector_AddElement(LFX_Vector* thiz, const void* element);
void LFX_Vector_SetElement(LFX_Vector* thiz, int index, const void* element);
void* LFX_Vector_GetElement(LFX_Vector* thiz, int index);
int LFX_Vector_GetCount(LFX_Vector* thiz);
