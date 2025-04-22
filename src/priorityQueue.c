#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priorityQueue.h"

typedef struct PriorityQueueInstance {
    size_t numElems;
    size_t elemSize;
    size_t capacity;
    void*  data;
    PQ_CompareFunCB cmpFunCB;
} PriorityQueueInstance;

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

static void Swap(PriorityQueue pq, void* a, void* b) {
    // Usa element 0 as a temp element
    memcpy(pq->data, a, pq->elemSize);
    memcpy(a, b, pq->elemSize);
    memcpy(b, pq->data, pq->elemSize);
}

PriorityQueue PQ_Create(size_t initialCapacity, size_t elementSize, PQ_CompareFunCB compareFunCB) {
    initialCapacity = NextPowerOf2((unsigned int)(initialCapacity+1));
	PriorityQueue inst = malloc(sizeof(PriorityQueueInstance));
	inst->capacity = initialCapacity;
    inst->elemSize = elementSize;
    inst->data     = malloc(initialCapacity * elementSize);
    inst->numElems = 0;
    inst->cmpFunCB = compareFunCB;

	return inst;
}

void PQ_Push(PriorityQueue pq, const void* newElement) {
    if (pq->numElems == (pq->capacity-1)) {
        void* newPtr = realloc(pq->data, pq->capacity*pq->elemSize*2);
        if (newPtr == NULL) {
            printf("Error: unable to allocate memory. Aborting\n");
            free(pq->data);
            abort();
        }
        else {
            pq->data = newPtr;
            pq->capacity = pq->capacity * 2;
        }
    }

    if (pq->numElems == 0) {
        memcpy((char *)pq->data + pq->elemSize, newElement, pq->elemSize); // The first element is stored on pos 1
    }
    else {
        size_t pos = pq->numElems+1;
        void* element = (char*)pq->data + (pos * pq->elemSize);
        memcpy(element, newElement, pq->elemSize);
        while (true) {
            size_t parentPos = (size_t)floor(pos / 2.0);
            void *parent = (char*)pq->data + (parentPos * pq->elemSize);
            if (pq->cmpFunCB(element, parent) <= 0)
                Swap(pq, element, parent);
            else
                break;

            if (parentPos == 1)
                break;
            else {
                element = parent;
                pos = parentPos;
            }
        }
    }
    pq->numElems++;
}

bool PQ_Pop(PriorityQueue pq, void* element) {
    if (pq->numElems == 0)
        return false;

    // Copy element to return
    void* parent = (char*)pq->data + pq->elemSize;
    memcpy(element, parent, pq->elemSize);

    // Replace root with last element
    void* last = (char*)pq->data + pq->numElems*pq->elemSize;
    memcpy(parent, last, pq->elemSize);
    pq->numElems--;
    size_t pos = 1;

    while (true) {
        size_t child1Pos = (size_t)floor(pos * 2.0);
        size_t child2Pos = child1Pos+1;

        void* child1 = NULL;
        void* child2 = NULL;

        if (child1Pos <= pq->numElems)
            child1 = (char*)pq->data + (child1Pos * pq->elemSize);

        if (child2Pos <= pq->numElems)
            child2 = (char*)pq->data + (child2Pos * pq->elemSize);
        
        if (!child1 && !child2)
            break;

        void* selChild = child1;
        size_t selChildPos = child1Pos;
        
        if (child2 != NULL && (pq->cmpFunCB(child1, child2) > 0)) {
            selChild = child2;
            selChildPos = child2Pos;
        }

        if (pq->cmpFunCB(parent, selChild) > 0)
            Swap(pq, parent, selChild);
        else
            break;

        parent = selChild;
        pos = selChildPos;
    }

    return true;
}

void PQ_Destroy(PriorityQueue pq) {
    free(pq->data);
	free(pq);
	pq = NULL;
}

size_t PQ_NumElements(PriorityQueue pq) {
    return pq->numElems;
}

const void* PQ_GetElement(PriorityQueue pq, size_t elementID) {
    if (elementID >= pq->numElems)
        return NULL;

    return (char*)pq->data + (elementID + 1) * pq->elemSize;
}

void PQ_Traverse(PriorityQueue pq, void (*funCB)(const void*)) {
    for (size_t i = 0; i < pq->numElems; i++) {
        funCB((char *)pq->data + (i+1) * pq->elemSize);
    }
}