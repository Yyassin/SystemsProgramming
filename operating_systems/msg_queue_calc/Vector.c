/**
 * Vector Header - Dynamically Resizing Collection
 * @Author: Yousef Yassin
 * @Date: November 23, 2021
 */

#include <stdio.h>

#include "Vector.h"

Vector* vec_allocate(int capacity)
{
    assert(capacity >= 0);

    Vector* vector = (Vector* )malloc(sizeof(Vector));
    assert(vector != NULL);

    // Initialize the vector.
    vector->capacity = capacity;
    vector->size = 0;
    vector->elems = (char* )malloc(capacity * sizeof(char));

    return vector;
}

void vec_destroy(Vector* vector)
{
    assert(vector != NULL);
    //printf("Cleanup vector.\n");
    free(vector->elems);
    free(vector);
}

void vec_print(const Vector* vector)
{
    assert(vector != NULL);

    printf("[");
    for (int i = 0; i < vector->size - 1; i++)
    {
        printf("%d, ", vector->elems[i]);
    }

    vector->size > 0 ? 
        printf("%d]\n", vector->elems[vector->size - 1]) :
        printf("]\n");
}

/**
 * @brief *Private Method* Increases the capacity
 * of the specified vector to new_capacity.
 * 
 * @param[inout] vector, the vector to increase the capacity of.
 * @param[in] new_capacity, the new capacity. 
 */
void _increase_capacity(Vector* vector, int new_capacity)
{
    // Ensure new cap is larger than current cap
    assert((vector != NULL) && (vector->capacity < new_capacity));

    // Allocate a new backing array
    char* new_vector = (char* )malloc(new_capacity * sizeof(char));
    assert(new_vector != NULL);

    // Copy the elems from the current one to the new one.
    for (int i = 0; i < vector->capacity; i++) {
		new_vector[i] = vector->elems[i];
	}

    // Update the capacity
    vector->capacity = new_capacity;

    // Free the old backing array, assign the new one to the vector.
	free(vector->elems);
	vector->elems = new_vector;
}

bool vec_pushback(Vector* vector, char elem)
{
    assert(vector != NULL);

    // If we don't have space, double the current capacity.
    if (vector->size == vector->capacity) {
        _increase_capacity(vector, 2 * vector->capacity);
    }

    // Append the specified elem and increment the size.
    vector->elems[vector->size++] = elem;
    return true;
}

int vec_capacity(const Vector* vector) {
    assert(vector != NULL);
    return vector->capacity;
}

int vec_size(const Vector* vector) {
    assert(vector != NULL);
    return vector->size;
}

int vec_get(const Vector* vector, int index)
{
    assert(vector != NULL);
    assert(index >= 0 && index < vector->size);
    return vector->elems[index];
}

void vec_swap(Vector* vector, int index_a, int index_b)
{
    assert(vector != NULL && index_a < vector->size && index_b < vector->size);
    char temp = vector->elems[index_a];
    // Swap a and b
    vector->elems[index_a] = vector->elems[index_b];
    vector->elems[index_b] = temp;
}

int vec_pop(Vector* vector) {
    int pop = vector->elems[vector->size - 1];
    vector->size--;
    return pop;
}
