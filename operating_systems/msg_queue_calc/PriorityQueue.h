/**
 * Priority Queue Header - Min/Max Heap
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#pragma once
#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "Vector.h"

// Type of heap 
enum HEAP_TYPE { MIN, MAX };

// Priority Queue Struct
typedef struct {
    Vector* items;              // Vector-based Binary Heap
    enum HEAP_TYPE heap_type;   // Priority Queue type (max/min)
} PriorityQueue;

/**
 * @brief Allocates and intializes a new priority queue.
 * 
 * @param[in] capacity The initializing capacity of the queue.
 * @param[in] heap_type The type of pqueue, MAX or MIN.
 * @return PriorityQueue*, the priority queue.
 */
PriorityQueue* priorityqueue_create(int capacity, const enum HEAP_TYPE heap_type);

/**
 * @brief Inserts a key into the specified priority queue,
 *  satisfying the queue type.
 * 
 * @param[inout] queue, the queue to insert to.
 * @param[in] key, the key to insert.
 */
void priorityqueue_insert(PriorityQueue* queue, int key);

/**
 * @brief Prints the specified priority queue.
 * 
 * @param[in] queue, the queue to print.
 */
void priorityqueue_print(const PriorityQueue* queue);

/**
 * @brief Pops the root element of the specified queue
 * and returns the value.
 * 
 * @param[inout] queue, the queue to pop.
 * @return int, the value of the root.
 */
int priorityqueue_pop_root(PriorityQueue* queue);

/**
 * @brief Returns the root element of the 
 * specified queue.
 * 
 * @param[in] queue, the queue to peek.
 * @return int, the element at the root of the queue.
 */
int priorityqueue_peek(const PriorityQueue* queue);

/**
 * @brief Returns the total space in the 
 * specified queue.
 * 
 * @param[in] queue, the queue to get the capacity of.
 * @return int, the capacity.
 */
int priorityqueue_capacity(const PriorityQueue* queue);

/**
 * @brief Returns the total number of
 * elements in the specified queue.
 * 
 * @param[in] queue, the queue to get the size of.
 * @return int, the size.
 */
int priorityqueue_size(const PriorityQueue* queue);

/**
 * @brief Returns the minimum number in a
 * max heap.
 * Precondition: The specified queue is of type MAX.
 * 
 * @param[in] queue, the maxheap queue to get the minimum from.
 * @return int, the minimum.
 */
int max_heap_get_min(const PriorityQueue* queue);

/**
 * @brief Deletes all instances of n in 
 * the specified priority queue.
 * 
 * @param[inout] queue, the priority queue to delete from.
 * @param[in] n, the number to delete.
 * @return int, the number of elements removed.
 */
int priorityqueue_delete(PriorityQueue* queue, int n);

/**
 * @brief Destroys and cleans up the
 * specified priority queue.
 * 
 * @param[in] queue, the queue to delete.
 */
void priorityqueue_destroy(PriorityQueue* queue);

#endif
