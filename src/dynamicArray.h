#ifndef __DYNAMIC_ARRAY_H__
#define __DYNAMIC_ARRAY_H__

#include <stdbool.h>

typedef struct DynamicArrayInstance* DynamicArray;

void         DA_Add(DynamicArray da, const void* element);                  // Adds an element to the end of the array
size_t       DA_Capacity(DynamicArray da);                                  // Gets the total number of elements the internal data structure can hold without resizing
void         DA_Clear(DynamicArray da);                                     // Removes all elements from the array
bool         DA_Contains(DynamicArray da, const void* element);             // Determines whether an element is in the array
size_t       DA_Count(DynamicArray da);                                     // Gets the number of elements contained in the array
DynamicArray DA_Create(size_t initialCapacity, size_t elementSize);         // Initializes a new instance of the array that can hold elements of size "elementSize", that is empty and has the specified "initialCapacity"
void         DA_Destroy(DynamicArray da);                                   // Releases the resources allocated by the array
const void*  DA_Get(DynamicArray da, size_t index);                         // Gets the element at the specified index
ptrdiff_t    DA_IndexOf(DynamicArray da, const void* element);              // Searches for the specified element and returns the zero-based index of the first occurrence within the entire array. Returns -1 if the element is not found
void         DA_Insert(DynamicArray da, size_t index, const void* element); // Inserts an element into the array at the specified index
ptrdiff_t    DA_LastIndexOf(DynamicArray da, const void* element);          // Searches for the specified element and returns the zero-based index of the last occurrence within the entire array. Returns -1 if the element is not found
bool         DA_Remove(DynamicArray da, const void* element);               // Removes the first occurrence of a specific element from the array
void         DA_RemoveAt(DynamicArray da, size_t index);                    // Removes the element at the specified index of the array
void         DA_Reverse(DynamicArray da);                                   // Reverses the order of the elements in the entire array
void         DA_Set(DynamicArray da, size_t index, const void* element);    // Sets the element at the specified index

#endif