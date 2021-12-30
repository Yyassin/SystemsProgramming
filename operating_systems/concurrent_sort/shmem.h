#include <stdbool.h>
#define SIZE 7  

// Shared memory struct
typedef struct st_shmem
{
    char arr[SIZE];     // Array to be sorted
    bool valid[3];      // Stores whether process P_i has finished 
                        // sorting for a cycle in valid[i - 1]
                        
    int readcount;      // Keeps count of readers in the "valid" critical section
} st_shmem;
