#include "ObjectPool.h"
#include "Context.h"

void LFX_ObjectPool_Init(LFX_ObjectPool* thiz, int object_size)
{
    memset(thiz, 0, sizeof(LFX_ObjectPool));
    thiz->object_size = object_size;
}

void LFX_ObjectPool_Done(LFX_ObjectPool* thiz)
{
    if (thiz->object_ptr_array)
    {
        LFX_LOGE("ObjectPool leak");

        LFX_ObjectPool_Release(thiz);
    }
}

void LFX_ObjectPool_Grow(LFX_ObjectPool* thiz)
{
    int old_size = thiz->array_size;
    if (old_size == 0)
    {
        thiz->array_size = 8;
    }
    else
    {
        thiz->array_size = old_size * 2;
    }

    void** new_array = malloc(sizeof(void*) * thiz->array_size);
    memset(new_array, 0, sizeof(void*) * thiz->array_size);
    if (old_size > 0)
    {
        memcpy(new_array, thiz->object_ptr_array, sizeof(void*) * old_size);
    }
    if (thiz->object_ptr_array)
    {
        free(thiz->object_ptr_array);
        thiz->object_ptr_array = NULL;
    }
    thiz->object_ptr_array = new_array;
}

void LFX_ObjectPool_Release(LFX_ObjectPool* thiz)
{
    for (int i = 0; i < thiz->array_size; ++i)
    {
        if (thiz->object_ptr_array[i])
        {
            free(thiz->object_ptr_array[i]);
            thiz->object_ptr_array[i] = NULL;
        }
    }
    thiz->object_count = 0;
    if (thiz->object_ptr_array)
    {
        free(thiz->object_ptr_array);
        thiz->object_ptr_array = NULL;
    }
    thiz->array_size = 0;
}

int LFX_ObjectPool_CreateObject(LFX_ObjectPool* thiz)
{
    if (thiz->array_size == thiz->object_count)
    {
        LFX_ObjectPool_Grow(thiz);
    }

    int index = -1;
    for (int i = 0; i < thiz->array_size; ++i)
    {
        if (thiz->object_ptr_array[i] == NULL)
        {
            index = i;
            break;
        }
    }

    thiz->object_ptr_array[index] = malloc(thiz->object_size);
    thiz->object_count += 1;

    int id = index + 1;
    return id;
}

void* LFX_ObjectPool_GetObject(LFX_ObjectPool* thiz, int id)
{
    if (id <= 0)
    {
        return NULL;
    }

    int index = id - 1;
    if (index >= thiz->array_size)
    {
        return NULL;
    }

    return thiz->object_ptr_array[index];
}

void LFX_ObjectPool_DestroyObject(LFX_ObjectPool* thiz, int id)
{
    int index = id - 1;
    free(thiz->object_ptr_array[index]);
    thiz->object_ptr_array[index] = NULL;
    thiz->object_count -= 1;

    if (thiz->object_count == 0)
    {
        LFX_ObjectPool_Release(thiz);
    }
}
