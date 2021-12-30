/**
 * Vector Header - Dynamically Resizing Collection
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#pragma once
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdlib.h> 
#include <stdio.h>
#include <stdbool.h>
#include <assert.h> 

// Vector struct
typedef struct {           
    char*  elems;  // The vector's backing array
    int capacity;  // The vector's total space
    size_t size;   // The vector's occupied space
} Vector;

/**
 * @brief Allocates and initializes 
 * a new vector with the specified capacity.
 * 
 * @param[in] capacity, the vector's capacity.
 * @return Vector*, the initialized vector.
 */
Vector* vec_allocate(int capacity);

/**
 * @brief Destroys and cleans up the 
 * specified vector.
 * 
 * @param[in] vector, the vector to destroy.
 */
void vec_destroy(Vector* vector);

/**
 * @brief Prints the contents of the 
 * specified vector.
 * 
 * @param[in] vector, the vector to print. 
 */
void vec_print(const Vector* vector);

/**
 * @brief Adds the specified element to the 
 * specified vector.
 * 
 * @param[inout] vector, the vector to add the element to.
 * @param[in] elem, the element to add.
 * @return true if elem added successfully, false otherwise.
 */
bool vec_pushback(Vector* vector, char elem);

/**
 * @brief Returns the capacity of the specified vector.
 * 
 * @param[in] vector, the vector to get the capacity of.
 * @return int, the capacity.
 */
int vec_capacity(const Vector* vector);

/**
 * @brief Returns the ssize of the specified vector.
 * 
 * @param[in] vector, the vector to get the size of.
 * @return int, the size.
 */
int vec_size(const Vector* vector);


/**
 * @brief Returns the value stored at the
 * specified index in the specified vector.
 * 
 * @param[in] vector, the vector to index.
 * @param[in] index, the index.
 * @return int, the value stored at the index.
 */
int vec_get(const Vector* vector, int index);

/**
 * @brief Removes and returns the value of the
 * last element in the specified vector.
 * 
 * @param[inout] vector, the vector to pop.
 * @return int, the popped value.
 */
int vec_pop(Vector* vector);

/**
 * @brief Swaps the elements at index_a and index_b
 * in the specified vector.
 * 
 * @param[inout] vector, the vector to swap elems in. 
 * @param[in] index_a, the first index. 
 * @param[in] index_b, the second index. 
 */
void vec_swap(Vector* vector, int index_a, int index_b);



#endif
