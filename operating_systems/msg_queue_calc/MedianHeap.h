/**
 * Median Heap Header
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#pragma once
#ifndef _MEDIAN_HEAP_H_
#define _MEDIAN_HEAP_H_

#include "PriorityQueue.h"

// Median Heap Struct
typedef struct {
    PriorityQueue* maxHeap; // Max heap of all elems < median
    PriorityQueue* minHeap; // Min heap of all elems > median
    int sum;                // Sum of all elements in the median heap
                            // for O(1) sum and average
} MedianHeap;

/**
 * @brief Allocates, initializes and returns
 * a new median heap with the specified capacity.
 * 
 * @param[in] capacity, the intiializing capacity for the median heap.
 * @return MedianHeap*, the new median heap.
 */
MedianHeap* medianheap_create(int capacity);

/**
 * @brief Inserts the specified number
 * into the median heap.
 * 
 * @param[inout] heap, the heap to insert into.
 * @param[in] n, the number to insert. 
 */
void medianheap_insert(MedianHeap* heap, int n);

/**
 * @brief Prints the median heap (its
 * two sub heaps).
 * 
 * @param[in] heap, the median heap to print. 
 */
void medianheap_print(const MedianHeap* heap);

/**
 * @brief Deletes all instances of n 
 * from the median heap.
 * 
 * @param[inout] heap, the heap to delete from.
 * @param[in] n, the number of delete.
 */
void medianheap_delete_all(MedianHeap* heap, int n);

/**
 * @brief Returns the median of all
 * the elements in the median heap.
 * 
 * @param[in] heap, the heap to get the median for.
 * @return double, the median.
 */
double medianheap_get_median(const MedianHeap* heap);

/**
 * @brief Gets the median of the median heap
 * if odd size. Else, gets the *two* elements
 * located at the middle of the sorted set of elements.
 * 
 * @param[in] heap, the heap to get the median for.
 * @param[out] medians, stores the median(s).
 * @return flag as true if two medians, else false.
 */
bool medianheap_get_median2(const MedianHeap* heap, int medians[]);

/**
 * @brief Returns the minimum value
 * in the median heap.
 * 
 * @param[in] heap, the heap to return the minimum for.
 * @return int, the minimum.
 */
int medianheap_get_min(const MedianHeap* heap);

/**
 * @brief Returns the sum of the 
 * elements in the median heap.
 * 
 * @param[in] heap, the heap to return the sum for.
 * @return int, the sum.
 */
int medianheap_get_sum(const MedianHeap* heap);

/**
 * @brief Returns the mean of the 
 * elements in the median heap.
 * 
 * @param[in] heap, the heap to return the average for.
 * @return double, the mean.
 */
double medianheap_get_average(const MedianHeap* heap);

/**
 * @brief Returns whether the median heap
 * is empty.
 * 
 * @param[in] heap, the heap to check if empty.
 * @return true if empty, else false.
 */
bool medianheap_is_empty(const MedianHeap* heap);


/**
 * @brief Destroys and cleans up the specified
 * median heap.
 * 
 * @param[in] heap, the median heap to destroy. 
 */
void medianheap_destroy(MedianHeap* heap);

#endif
