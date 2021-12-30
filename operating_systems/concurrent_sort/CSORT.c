#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>

#include "sharedMemoryWrapper.h"
#include "semWrapper.h"
#include "shmem.h"

/* Utility Macros */
#define ASSERT(condition, msg) { if(!(condition)) { printf("Assert Failed: %s\n", msg); exit(1); } }
#define DPRINTF(args...) { if(debug_mode) { printf(args); } }

/* Constants and enums */
const int TOTAL_SEM = 2;
const int TOTAL_PROCESSES = 3;
enum PROCESS_IDX { P1, P2, P3 }; // 0, 1, 2
enum MUTEX { LEFT_WRITE, RIGHT_WRITE };  // 0, 1, 2

/**
 * @brief Prompts the user to select the 
 * running mode: DEBUG or PRODUCTION.
 */ 
static bool debug_mode = false;
void debug_prompt(void) {
    // Prompt and get response
	char response[4]; 
	printf("Would you like to run in debug mode (yes/no)? ");
    while (true) {
        scanf(" %s", response);
        if (strcmp("yes", response) == 0) {
            printf("Ok, running in debug.\n");
            debug_mode = true;
            return;
        } else if (strcmp("no", response) == 0) {
            printf("Ok, running in production.\n");
            return;
        } 
        printf("Please enter a valid input (yes/no).\n");
        printf("Would you like to run in debug mode (yes/no)? ");
    }
}

/**
 * @brief Waits for num_processes processes to terminate.
 * @param[in] num_process int, the number of processes to wait for.
 */ 
void wait_n_children(int num_process)
{
    int status;
    pid_t child_pid;

    while (num_process > 0)
    {
        child_pid = wait(&status);
        // DPRINTF("[Debug] Child (PID: %d) exited with status code 0x%x.\n", child_pid, status);
        num_process--;
    }
}

/**
 * @brief Prompts the users to enter n characters
 * to initialize the first n elements of arr.
 * @param[in] arr char*, the array.
 * @param[in] n int, the number of cells to initialize.
 */ 
void init_array(char* arr, int n)
{
    /* Precondition: Assumes 1 char inputs */
    printf("Please enter %d letters to sort: \n", n);
    
    for (int i = 0; i < n; i++)
    {
        arr[i] = 0;
        bool ischar = isalpha(arr[i]);

        // Keep looping on elem i until 
        // it's a valid char.
        while (!ischar)
        {
            printf("Character %d: ", i + 1);
            scanf(" %c", &arr[i]);

            if (!(ischar = isalpha(arr[i])))
                printf("\nPlease enter a valid character!\n");
        }
    }
}

/**
 * @brief (Re)sets the elements of the boolean
 * array arr all to false.
 * @param[in] arr bool*, the array.
 * @param[in] n int, the number of elements to check.
 */ 
void reset(bool* arr, int n, int type)
{
    int start = 0;
    int end = n;
    if (type == 1) { end = 2; }
    else if (type == 3) { start = 1;}
    for (int i = start; i < end; i++)
    {
        arr[i] = false;
    }
}

/**
 * @brief Checks if the first n cells in valid array
 * are true -> this will happen when all processes have sorted
 * their assigned elements (and we're finished).
 * @param[in] shmem st_shmem, the shared memory.
 * @param[in] n int, the number of cells to check.
 * @param[in] mutex int, the CS's semaphore lock.
 * @returns true if all valid, false otherwise.
 */ 
bool validate(st_shmem* shmem, int n, int mutex)
{
    bool ret = true;

    for (int i = 0; i < n; i++){
        if (!shmem->valid[i]) 
        {
            ret = false;
            break;
        }
    }

    return ret;
}

/**
 * @brief Returns whether the process p_i has completed
 * its current sorting iteration.
 * @param[in] shmem st_shmem*, the shared memory.
 * @param[in] i int, the process index.
 * @param[in] mutex int, the CS's semaphore lock.
 * @returns true if valid, false otherwise.
 */ 
bool valid_iteration(st_shmem* shmem, int i, int mutex)
{
    // Block writers since were reading
    // (Pi will reset all valid cells once it swaps)
    bool ret = shmem->valid[i];

    return ret;
}

/**
 * @brief Prints the first n elements
 * of the array arr.
 * @param[in] arr char*, the array.
 * @param[in] n int, the number of elements to print.
 */ 
void print_array(char* arr, int n)
{
    printf("[ ");
    for (int i = 0; i < n; i++)
    {
        printf("%c ", arr[i]);
    }
    printf("]\n");
}

/**
 * @brief Performs bubble sort on the shared array
 * from index start to end inclusive.
 * @param[in] shmem st_shmem*, the shared memory.
 * @param[in] start int, the starting index.
 * @param[in] end int, the ending index.
 * @param[in] mutex int, the CS's semaphore lock.
 */ 
void sort(st_shmem* shmem, int start, int end, int mutex)
{
    int process_num = start / 2 + 1;
    char* arr = shmem->arr;
    bool* valid = shmem->valid;
    int temp = 0;
    int swaps = true;
    
    while (swaps)
    {
        swaps = false;
        arr[start] = tolower(arr[start]);
        for (int i = start + 1; i <= end; i++)
        {          
            arr[i] = tolower(arr[i]);
 
            if (i == 2 || i == 3)
            {
                ASSERT(sem_wait(mutex, LEFT_WRITE) != -1, "Wait left.");
            } else if (i == 4 || i == 5) {
                ASSERT(sem_wait(mutex, RIGHT_WRITE) != -1, "Wait right.");
            }

            if (arr[i] < arr[i - 1])
            {
                DPRINTF("[Debug] Process P%d: performed swapping.\n", process_num);
                swaps = true;
                temp = arr[i];
                arr[i] = arr[i - 1];
                arr[i - 1] = temp;

                reset(valid, 3, process_num);
            } 
            else 
            {
                DPRINTF("[Debug] Process P%d: performed no swapping.\n", process_num);
            }

            if (i == 2 || i == 3)
            {
                ASSERT(sem_signal(mutex, LEFT_WRITE) != -1, "Signal left.");
            } else if (i == 4 || i == 5) {
                ASSERT(sem_signal(mutex, RIGHT_WRITE) != -1, "Signal right.");
            }
        }
    }

    valid[start / 2] = true;
    /**
     * Valid doesn't need a semaphore since it is checked in our while loop.
     * If all processes are valid, we are done (they all finished without swapping).
     * Otherwise, any swap will be caught in the loop since we won't exit.
     */
}

/**
 * @brief Process work loop. Keeps swapping elements
 * in process' allocated range until all work flag 
 * complete at the same time.
 * @param process_idx int, the starting index for the process' range.
 * @param[in] shmem st_shmem*, the shared memory.
 * @param[in] mutex int, the CS's semaphore lock.
 */ 
void do_work(int process_idx, st_shmem* st_shared, int mutex)
{
    // While there's atleast one process that isn't finished
    while (!validate(st_shared, 3, mutex)) {    
        // Skip if this process has completed a valid iteration
        if (!valid_iteration(st_shared, process_idx, mutex))    
        {
            // Otherwise sort
            /* Let the index be the process number - 1 then
             * each process if reponsible for [2 * index, 2 * (index + 1)]
             * P1≡0 [0, 2], P2≡1 [2, 4], P3≡2 [4, 6]
             */
            sort(st_shared, 2 * process_idx, 2 * process_idx + 2, mutex);
        }
    }
}

int main(void)
{
    pid_t pids[TOTAL_PROCESSES];                        // Process IDs
    int mutex;                                          // Semaphore IDs
    ASSERT((mutex = sem_create((key_t)1234, TOTAL_SEM)) != -1, "Create semaphores");

    // Initialize semaphores to 1
    for (int i = 0; i < TOTAL_SEM; i++) {
        ASSERT(sem_set(mutex, i, 1) != -1, "Set semaphore");
    }

    // Create Shared Memory
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    int shmid;

    ASSERT((shmid = shmem_create(IPC_PRIVATE, sizeof(st_shmem))) != -1, "Create shared memory.");   // Create the shared memory
    ASSERT((shared_memory = shmem_attach(shmid)) != -1, "Create shared memory."); // Attach the shared memory to the parent
    st_shmem* st_shared = (st_shmem *)shared_memory; 

    // Initialization
    debug_prompt();                     // Prompt the user to select the run mode
    st_shared->readcount = 0;           // Total "valid state" readers is 0
    init_array(st_shared->arr, SIZE);   // Initialize the array to be sorted
    reset(st_shared->valid, TOTAL_PROCESSES, 2);      // Set valid state to all false / 0

    // Create child processes.
    ASSERT((pids[P1] = fork()) > -1, "Fork first child.");   // Fork failed
    if (pids[P1] > 0)
    {
        ASSERT((pids[P2] = fork()) > -1, "Fork first child.");   // Fork failed
        if (pids[P2] > 0)
        {
            ASSERT((pids[P3] = fork()) > -1, "Fork third child.");   // Fork failed
            if (pids[P3] > 0)
            {
                // Parent Process
                wait_n_children(TOTAL_PROCESSES);   // Wait for three children to exit
                printf("Sorted Array: ");
                print_array(st_shared->arr, SIZE);  // Print the resulting sorted array
                ASSERT(sem_delete(mutex) != -1, "Delete semaphores."); // Delete the shared memory
                ASSERT(shmem_delete(shmid) != -1, "Dettach shared memory."); // Delete the semaphores
            }
            else 
            {
                // Process P3
                ASSERT((shared_memory = shmem_attach(shmid)) != -1, "Create shared memory."); // Attach the shared memory to the parent
                do_work(P3, st_shared, mutex);          // Enter sorting loop
                ASSERT(shmem_dettach(st_shared) != -1, "Dettach shared memory."); // Dettach the shared memory
            }
        }
        else
        {
            // Process P2
            ASSERT((shared_memory = shmem_attach(shmid)) != -1, "Create shared memory."); // Attach the shared memory to the parent
            do_work(P2, st_shared, mutex);              // Enter sorting loop
            ASSERT(shmem_dettach(st_shared) != -1, "Dettach shared memory."); // Dettach the shared memory
        }
    }
    else
    {
        // Process P1
        ASSERT((shared_memory = shmem_attach(shmid)) != -1, "Create shared memory."); // Attach the shared memory to the parent
        do_work(P1, st_shared, mutex);                  // Enter sorting loop
        ASSERT(shmem_dettach(st_shared) != -1, "Dettach shared memory."); // Dettach the shared memory
    }

    exit(EXIT_SUCCESS);
}
