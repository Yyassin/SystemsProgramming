/**
 * Defines the structure of the shared memory block.
 * @Author: Yousef Yassin
 * @Date: October 26 2021
 */ 

#include <stdbool.h>
#define M_SIZE 3

typedef struct st_shmem
{
    double M[M_SIZE][M_SIZE];  // 3x3 Matrix indexed (row, col)
    double D[M_SIZE];          // Cofactor for each col
    double L[M_SIZE];          // Largest element in row
    bool valid[M_SIZE];        // Init 0, set to 1 on first placed data (i.e valid)
                               // Since the last process will print (not the parent).
} st_shmem;

