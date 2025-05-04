#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamicArray.h"

typedef struct DynamicArrayInstance {
    size_t count;
    size_t capacity;
    size_t elemSize;
    void*  data;
} DynamicArrayInstance;

static unsigned int NextPowerOf2(unsigned int n) {
    if (n == 0) return 1;

    n--;                        // Resta 1 para manejar casos donde n ya es potencia de 2
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

static void ResizeIfNeeded(DynamicArray da) {
    if (da->count >= da->capacity) {
        void* newPtr = realloc(da->data, da->capacity * da->elemSize * 2);
        if (newPtr == NULL) {
            printf("Error: unable to allocate memory. Aborting\n");
            free(da->data);
            abort();
        }
        else {
            da->data = newPtr;
            da->capacity = da->capacity * 2;
        }
    }
}

void DA_Add(DynamicArray da, const void* element) {
    ResizeIfNeeded(da);
    memcpy((char*)da->data + da->count*da->elemSize, element, da->elemSize);
    da->count++;
}

size_t DA_Capacity(DynamicArray da) {
    return da->capacity;
}

void DA_Clear(DynamicArray da) {
    da->count = 0;
}

bool DA_Contains(DynamicArray da, const void* element) {
    return DA_IndexOf(da, element) >= 0;
}

size_t DA_Count(DynamicArray da) {
    return da->count;
}

DynamicArray DA_Create(size_t initialCapacity, size_t elementSize) {
    initialCapacity = NextPowerOf2((unsigned int)(initialCapacity));
    DynamicArray inst = malloc(sizeof(DynamicArrayInstance));
    inst->capacity = initialCapacity;
    inst->elemSize = elementSize;
    inst->data = malloc(initialCapacity * elementSize);
    inst->count = 0;

    return inst;
}

void DA_Destroy(DynamicArray da) {
    if (da->data) {
        free(da->data);
        da->data = NULL;
        da->count = 0;
        da->capacity = 0;
        da->elemSize = 0;
    }
}

const void* DA_Get(DynamicArray da, size_t index) {
    assert(index >= 0 && index < da->count && "DA_Get: index out of bounds");
    return (char*)da->data + da->elemSize * index;
}

ptrdiff_t DA_IndexOf(DynamicArray da, const void* element) {
    for (int i = 0; i < da->count; i++) {
        if (memcmp(element, (char*)da->data + da->elemSize * i, da->elemSize) == 0)
            return i;
    }
    return -1;
}

void DA_Insert(DynamicArray da, size_t index, const void* element) {
    assert(index >= 0 && index <= da->count && "DA_Insert: index out of bounds");
    ResizeIfNeeded(da);
    if (index < da->count)
        memmove((char*)da->data + (index+1)*da->elemSize, (char*)da->data + index*da->elemSize, (da->count - index)*da->elemSize);
    memcpy((char*)da->data + index * da->elemSize, element, da->elemSize);
    da->count++;
}

ptrdiff_t DA_LastIndexOf(DynamicArray da, const void* element) {
    for (int i = da->count-1; i >= 0; i--) {
        if (memcmp(element, (char*)da->data + da->elemSize * i, da->elemSize) == 0)
            return i;
    }
    return -1;
}

bool DA_Remove(DynamicArray da, const void* element) {
    ptrdiff_t index = DA_IndexOf(da, element);
    if (index >= 0) {
        DA_RemoveAt(da, index);
        return true;
    }

    return false;
}

void DA_RemoveAt(DynamicArray da, size_t index) {
    assert(index >= 0 && index < da->count && "DA_RemoveAt: index out of bounds");
    if (index < da->count - 1)
        memmove((char*)da->data + index * da->elemSize, (char*)da->data + (index+1) * da->elemSize, (da->count - index)*da->elemSize);

    da->count--;
}

void DA_Reverse(DynamicArray da) {
    void* temp = malloc(da->elemSize);
    void* head = da->data;
    void* tail = (char *)da->data + (da->count-1) * da->elemSize;

    for (int i = 0; i < (da->count / 2); i++) {
        memcpy(temp, head, da->elemSize);
        memcpy(head, tail, da->elemSize);
        memcpy(tail, temp, da->elemSize);

        head = (char*)head + da->elemSize;
        tail = (char*)tail - da->elemSize;
    }

    free(temp);
}

void DA_Set(DynamicArray da, size_t index, const void* element) {
    assert(index >= 0 && index < da->count && "DA_Set: index out of bounds");
    memcpy((char*)da->data + index * da->elemSize, element, da->elemSize);
}

