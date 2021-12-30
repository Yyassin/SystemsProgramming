/**
 * Priority Queue - Min/Max Heap
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#include "PriorityQueue.h"

// Helper macro to get minimum of two numbers a and b
#define min(a,b) (((a) < (b)) ? (a) : (b))

/**
 * @brief Recursively heapifies (percolates) the specified 
 * node down the specified queue's binary heap.
 * 
 * Used for popping roots, swap root with last leaf
 * 
 * @param[inout] queue, the queue to heapify.
 * @param[in] parent_node, the node to heapify down.
 */
void _heapify_top_bottom(PriorityQueue* queue, int parent_node) {
    assert(queue != NULL);
    int left = parent_node * 2 + 1;     // Property: left @ 2p + 1
    int right = parent_node * 2 + 2;    // Property: right @ 2p + 2
    int minmax;                         // Current min/max (min for MIN heap, max for MAX heap)

    // Reference
    Vector* vec = queue->items;
    int size = vec_size(vec);

    // If there's a left child, check if it does not satisfy the heap property.
    bool predicate = (queue->heap_type == MIN) ?
        !(left >= size || left < 0) && vec_get(vec, left) < vec_get(vec, parent_node)  // Min: ! child > parent
        :
        !(left >= size || left < 0) && vec_get(vec, left) > vec_get(vec, parent_node); // Max: ! child < parent

    // If the heap property is not satisfied, assign
    // the child to minmax.
    if (predicate) {
        minmax = left;
    } else {
        minmax = parent_node;
    }

    // Repeat for right child...
    // If there's a right child, check if it does not satisfy the heap property.
    predicate = (queue->heap_type == MIN) ? 
        !(right >= size || right < 0) && vec_get(vec, right) < vec_get(vec, minmax)    // Min: ! child > parent
        :
        !(right >= size || right < 0) && vec_get(vec, right) > vec_get(vec, minmax);   // Max: ! child < parent

    // If the heap property is not satisfied, assign
    // the child to minmax.
    if (predicate){
        minmax = right;
    }

    /** If the minmax was not the parent, the heap property
     * was not satisfied and we have the smallest/largest of the children
     * in minmax. Swap it with the parent and then continue
     * percolating down from that index.
     */ 
    if (minmax != parent_node) {
        vec_swap(vec, parent_node, minmax);
        _heapify_top_bottom(queue, minmax);
    }
    // Otherwise we're done.
}

/**
 * @brief Recursively heapifies (percolates) the specified 
 * node up the specified queue's binary heap.
 * 
 * Used for inserts, add to end then bubble up
 * 
 * @param[inout] queue, the queue to heapify.
 * @param[in] parent_node, the node to heapify up.
 */
void _heapify_bottom_top(PriorityQueue* queue, int index) {
    assert(queue != NULL);
    // Property: parent @ (i - 1) / 2
    int parent_node = (index - 1) / 2;  

    // Reference
    Vector* vec = queue->items;

    // Check if heap structure is *not* satisfied
    // MIN HEAP: parent is smaller or equal than child
    // MAX HEAP: parent is larger or equal than child
    bool predicate = (queue->heap_type == MIN) ?
        vec_get(vec, parent_node) > vec_get(vec, index) 
        :
        vec_get(vec, parent_node) < vec_get(vec, index);

    if (predicate) {
        // Heap structure not satisfied, swap 
        // the parent and child to restore.
        vec_swap(vec, parent_node, index);
        // Continue bubbling up from the parent index
        // until no swaps
        _heapify_bottom_top(queue, parent_node);
    }
}

/**
 * @brief Rebuild the specified queue's 
 * items queue to satisfy the associated heap property.
 * 
 * Used to reestablish the heap property after 
 * a delete (not a pop).
 * 
 * @param[inout] queue, the queue to rebuild.
 */
void _rebuild_heap(PriorityQueue* queue) {
    assert(queue != NULL);
    // Percolate up from every leaf from the
    // bottom row and every node will fall into its place.
    int start = vec_size(queue->items) / 2 - 1;

    for (int i = start; i >= 0; i--)
    {
        _heapify_top_bottom(queue, i);
    }
}

PriorityQueue* priorityqueue_create(int capacity, const enum HEAP_TYPE heap_type) 
{
    PriorityQueue* queue = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    assert(queue != NULL);

    // Initialize the priority queue
    queue->heap_type = heap_type;
    queue->items = vec_allocate(capacity);
    return queue;
}

void priorityqueue_insert(PriorityQueue* queue, int key) {
    assert(queue != NULL);
    // Append the element to the last leaf (bottom right) of the heap, 
    // then percolate it up until it reaches the correct location
    vec_pushback(queue->items, key);
    _heapify_bottom_top(queue, queue->items->size - 1);
}

int priorityqueue_delete(PriorityQueue* queue, int key) {
    assert(queue != NULL);
    // Create a new vector to store the filtered 
    // elements (will filter out key).
    Vector* filtered = vec_allocate(queue->items->capacity);

    int num_removed = 0;
    int curr_elem;

    // Go through the current heap and leave out 
    // all instances of key in the new array
    for (int i = 0; i < queue->items->size; i++) {
        curr_elem = vec_get(queue->items, i);
        if (curr_elem != key) {
            vec_pushback(filtered, curr_elem);
        } else {
            num_removed++;
        }
    }

    if (num_removed > 0)
    {
        // Something was removed, free the 
        // old heap and assign the new filtered one
        vec_destroy(queue->items);
        queue->items = filtered;
        // Then rebuild to restore heap structure
        _rebuild_heap(queue);
    } else {
        // Otherwise, keep the current heap and free 
        // filtered.
        vec_destroy(filtered);
    }

    return num_removed;
}

int priorityqueue_pop_root(PriorityQueue* queue) {
    assert(queue != NULL);
    assert(queue->items->size > 0);
    int pop;

    Vector* vec = queue->items;
    pop = vec_get(vec, 0);              // Get value of root
    vec_swap(vec, 0, vec->size - 1);    // Swap root with last leaf
    vec_pop(vec);                       // Remove last elem (the root)
    _heapify_top_bottom(queue, 0);      // Percolate the last leaf down

    return pop;
}

int max_heap_get_min(const PriorityQueue* queue) {
    assert(queue != NULL && vec_size(queue->items) > 0);
    assert(queue->heap_type == MAX);    // Ensure this is a max heap

    int n = vec_size(queue->items);

    // For a max heap, the minimum will be in the last row (second half of the array)
    int minimum_elem = vec_get(queue->items,  n / 2);
    // Go through the last row and keep updating the minimum
    for (int i = 1 + n / 2; i < n; i++)
    {
        minimum_elem = min(minimum_elem, vec_get(queue->items, i));
    }

    return minimum_elem;
}

int priorityqueue_peek(const PriorityQueue* queue) {
    assert(queue != NULL);
    assert(vec_capacity(queue->items) > 0);

    return vec_get(queue->items, 0);
}

void priorityqueue_print(const PriorityQueue* queue) {
    assert(queue != NULL);
    vec_print(queue->items);
}

int priorityqueue_capacity(const PriorityQueue* queue) {
    assert(queue != NULL);
    return vec_capacity(queue->items);
}

int priorityqueue_size(const PriorityQueue* queue) {
    assert(queue != NULL);
    return vec_size(queue->items);
}

void priorityqueue_destroy(PriorityQueue* queue) {
    assert(queue != NULL);
    //printf("Cleanup pqueue.\n");
    vec_destroy(queue->items);
    free(queue);
}
