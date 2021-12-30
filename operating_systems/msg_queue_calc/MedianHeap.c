/**
 * Median Heap 
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#include "MedianHeap.h"
#include <stdlib.h>

/**
 * @brief Recursively rebalances the median heap 
 * to ensure that the maximum difference in size
 * between the min and max heaps is 1.
 * 
 * Used after insertions and deletes.
 * 
 * @param[inout] heap, the heap to rebalance.
 */
void _rebalance(MedianHeap* heap)
{
    assert(heap != NULL);
    // If the difference between the two heaps is greater than 1
    if (abs(priorityqueue_size(heap->maxHeap) - priorityqueue_size(heap->minHeap)) > 1)
    {
        if (priorityqueue_size(heap->maxHeap) > priorityqueue_size(heap->minHeap))
        {
            // Max heap has the extra element(s), then pop the max from max and add to min
            priorityqueue_insert(heap->minHeap, priorityqueue_pop_root(heap->maxHeap));
        }
        else
        {
            // Min heap has the extra element(s), then pop the min from min and add to max
            priorityqueue_insert(heap->maxHeap, priorityqueue_pop_root(heap->minHeap));
        }
    }
    // If the difference between the heap size is still greater than 1, rebalance again.
    if (abs(priorityqueue_size(heap->maxHeap) - priorityqueue_size(heap->minHeap)) > 1) _rebalance(heap);
}

MedianHeap* medianheap_create(int capacity)
{
    MedianHeap* heap = (MedianHeap *)malloc(sizeof(MedianHeap));
    assert(heap != NULL);

    // Initialize the median heap
    heap->maxHeap = priorityqueue_create(capacity, MAX);
    heap->minHeap = priorityqueue_create(capacity, MIN);
    heap->sum = 0;
    return heap;
}

void medianheap_insert(MedianHeap* heap, int n)
{
    assert(heap != NULL);
    if (medianheap_is_empty(heap)) 
    { 
        // If the median heap is empty, anything is larger
        // than the median -> goes to minheap
        priorityqueue_insert(heap->minHeap, n); 
    }
    else
    {
        // Otherwise, add numbers > median to max heap
        // and < median to min heap
        if (n < medianheap_get_median(heap))
        {
            priorityqueue_insert(heap->maxHeap, n);
        }
        else
        {
            priorityqueue_insert(heap->minHeap, n);
        }
    }

    heap->sum += n;
    _rebalance(heap);   // Rebalance the median heap if needed
}

void medianheap_print(const MedianHeap* heap)
{
    assert(heap != NULL);
    printf("Less than median, max heap: \n");
    priorityqueue_print(heap->maxHeap);
    printf("Greater than median: \n");
    priorityqueue_print(heap->minHeap);
}

void medianheap_delete_all(MedianHeap* heap, int n)
{
    /** To delete all instances of n, delete all
     * instances of n in the sub heaps. Note that n can be in both 
     * (consider all elements are equal to n).
     */
    assert(heap != NULL);
    int total_deleted = 0;
    total_deleted += priorityqueue_delete(heap->maxHeap, n);
    total_deleted += priorityqueue_delete(heap->minHeap, n);

    heap->sum -= total_deleted * n; // Update the sum
    _rebalance(heap);
}

double medianheap_get_median(const MedianHeap* heap)
{
    assert(heap != NULL);
    // If both heaps are equal in size, the two middle elements
    // are the roots of both heaps. Median is their average.
    if (priorityqueue_size(heap->maxHeap) == priorityqueue_size(heap->minHeap)) 
    {
        return ((double)priorityqueue_peek(heap->maxHeap) + (double)priorityqueue_peek(heap->minHeap)) / 2;
    }
    // Otherwise, if the max heap is larger (ensured to be by 1), the median is its root.
    else if (priorityqueue_size(heap->maxHeap) > priorityqueue_size(heap->minHeap))
    {
        return (double)priorityqueue_peek(heap->maxHeap);
    }
    
    // Otherwise, the min heaps is larger by 1, the median is its root.
    return (double)priorityqueue_peek(heap->minHeap);
}

bool medianheap_get_median2(const MedianHeap* heap, int medians[])
{
    assert(heap != NULL);
    // If both heaps are equal in size, the two middle elements
    // are the roots of both heaps. Return both.
    if (priorityqueue_size(heap->maxHeap) == priorityqueue_size(heap->minHeap)) 
    {
        medians[0] = (double)priorityqueue_peek(heap->maxHeap);
        medians[1] = (double)priorityqueue_peek(heap->minHeap);

        return true;
    }
    // Otherwise, if the max heap is larger (ensured to be by 1), the median is its root.
    else if (priorityqueue_size(heap->maxHeap) > priorityqueue_size(heap->minHeap))
    {
        medians[0] = (double)priorityqueue_peek(heap->maxHeap);
        
    }
    // Otherwise, the min heaps is larger by 1, the median is its root.
    else {
        medians[0] = (double)priorityqueue_peek(heap->minHeap);
    }
    return false;
}

int medianheap_get_min(const MedianHeap* heap)
{
    assert(heap != NULL && !(medianheap_is_empty(heap)));
    // The minimum is in the max heap, unless its empty
    if (priorityqueue_size(heap->maxHeap) == 0) { return priorityqueue_peek(heap->minHeap); }
    return max_heap_get_min(heap->maxHeap);
}

bool medianheap_is_empty(const MedianHeap* heap)
{
    assert(heap != NULL);
    return (priorityqueue_size(heap->minHeap) == 0) && (priorityqueue_size(heap->maxHeap) == 0);
}

int medianheap_get_sum(const MedianHeap* heap)
{
    assert(heap != NULL);
    return heap->sum;
}

double medianheap_get_average(const MedianHeap* heap)
{
    assert(heap != NULL);
    int total_elems = priorityqueue_size(heap->maxHeap) + priorityqueue_size(heap->minHeap);
    return (double)heap->sum / total_elems; // Should be sum method
}

void medianheap_destroy(MedianHeap* heap) {
    assert(heap != NULL);
    //printf("Cleanup median heap.\n");
    priorityqueue_destroy(heap->maxHeap);
    priorityqueue_destroy(heap->minHeap);
    free(heap);
}
