#include "Vector.h"
#include "Context.h"

void LFX_Vector_Init(LFX_Vector* thiz, int sizeof_element)
{
    memset(thiz, 0, sizeof(LFX_Vector));
    thiz->count = 0;
    thiz->sizeof_element = sizeof_element;
    thiz->capacity = 8;
    thiz->data = malloc(thiz->sizeof_element * thiz->capacity);
}

void LFX_Vector_Done(LFX_Vector* thiz)
{
    if (thiz->data)
    {
        free(thiz->data);
    }
    memset(thiz, 0, sizeof(LFX_Vector));
}

void LFX_Vector_AddElement(LFX_Vector* thiz, const void* element)
{
    if (thiz->count >= thiz->capacity)
    {
        thiz->capacity *= 2;
        thiz->data = realloc(thiz->data, thiz->sizeof_element * thiz->capacity);
    }

    int index = thiz->count;
    thiz->count += 1;
    LFX_Vector_SetElement(thiz, index, element);
}

void LFX_Vector_SetElement(LFX_Vector* thiz, int index, const void* element)
{
    if (index < 0 || index >= thiz->count)
    {
        LFX_LOGE("Vector index out of range: %d", index);
        return;
    }

    uint8_t* p = (uint8_t*) thiz->data;
    memcpy(&p[thiz->sizeof_element * index], element, thiz->sizeof_element);
}

void* LFX_Vector_GetElement(LFX_Vector* thiz, int index)
{
    if (index < 0 || index >= thiz->count)
    {
        LFX_LOGE("Vector index out of range: %d", index);
        return NULL;
    }

    uint8_t* p = (uint8_t*) thiz->data;
    return &p[thiz->sizeof_element * index];
}

int LFX_Vector_GetCount(LFX_Vector* thiz)
{
    return thiz->count;
}
