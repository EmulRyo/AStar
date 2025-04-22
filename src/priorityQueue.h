#ifndef __PRIORITY_QUEUE_H__
#define __PRIORITY_QUEUE_H__

typedef struct PriorityQueueInstance* PriorityQueue;
typedef int (*PQ_CompareFunCB) (const void*, const void*);

// It creates a priority queue using a binary heap
PriorityQueue PQ_Create(size_t initialCapacity, size_t elementSize, PQ_CompareFunCB compareFunCB);
void          PQ_Push(PriorityQueue pq, const void* newElement);
bool          PQ_Pop(PriorityQueue pq, void* element);
size_t        PQ_NumElements(PriorityQueue pq);
const void*   PQ_GetElement(PriorityQueue pq, size_t elementID);
void          PQ_Traverse(PriorityQueue pq, void (*funCB)(const void*));
void          PQ_Destroy(PriorityQueue pq);

#endif