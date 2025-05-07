#ifndef __PRIORITY_QUEUE_H__
#define __PRIORITY_QUEUE_H__

typedef struct PriorityQueueInstance* PriorityQueue;
typedef int (*PQ_CompareFunCB) (const void*, const void*); // Function to compare 2 elements. It should return an integer: negative value prioritizes the first element, positive value prioritizes the second element, 0 means both elements have same priority

size_t        PQ_Capacity(PriorityQueue pq);                             // Gets the total number of elements the internal data structure can hold without resizing
void          PQ_Clear(PriorityQueue pq);                                // Removes all elements from the PriorityQueue
size_t        PQ_Count(PriorityQueue pq);                                // Gets the number of elements contained on the PriorityQueue
PriorityQueue PQ_Create(size_t initialCapacity, size_t elementSize, PQ_CompareFunCB compareFunCB); // It creates a PriorityQueue using a binary heap. compareFunCB is used to order the elements on the PriorityQueue
void          PQ_Destroy(PriorityQueue pq);                              // Releases the resources allocated by the priorityQueue
const void*   PQ_Get(PriorityQueue pq, size_t index);                    // Gets the element at the specified index
bool          PQ_Pop(PriorityQueue pq, void* element);                   // Removes and returns the minimal element from the PriorityQueue that is, the element with the lowest priority value
void          PQ_Push(PriorityQueue pq, const void* newElement);         // Adds the specified element to the PriorityQueue
void          PQ_Traverse(PriorityQueue pq, void (*funCB)(const void*)); // It calls "funCB(const void *element)" as many times as elements exists on the PriorityQueue

#endif