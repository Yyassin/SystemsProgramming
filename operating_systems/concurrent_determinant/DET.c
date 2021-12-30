/**
 * Concurrent Determinant Calculation
 * @Author: Yousef Yassin
 * @Date: October 26, 2021
 */

/**
 * Determinant Background: 
 * 
 * | a b c |
 * | d e f |
 * | g h i |
 * 
 * Cofactor expansion along row 1 gives
 * D = a(1)| e f | + b(-1)| d f | + c(1)| d e |
 *         | h i |        | g i |       | g h |
 * 
 * D[0] = a(ei - fh), D[1] = (fg - di), D[2] = c(dh - ge)
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "shared_memory.h"

#define MICRO_SEC_IN_SEC 1000000

// Shared Memory Macros
#define ASSERT(condition, msg) { if(!(condition)) { printf("Assert Failed: %s\n", msg); exit(1); } }
#define ATTACH_MEM(shmid, name) { ASSERT((shared_memory = shmat(shmid, (void *)0, 0)) != (void *)-1, "Attach shared memory (" #name ")."); }
#define DETTACH_MEM(shared, name) { ASSERT(shmdt((void *)shared) != -1, "Dettach shared memory (" #name ")."); }

/**
 * @brief Waits for num_processes processes to terminate.
 */ 
void wait_n_children(int num_process)
{
    int status;
    pid_t child_pid;

    while (num_process > 0)
    {
        child_pid = wait(&status);
        //printf("Child (PID: %d) exited with status code 0x%x.\n", child_pid, status);
        num_process--;
    }
}

/**
 * @brief Prints the status of a child process.
 */ 
void print_status(int idx) { printf("Child process: working with element %d of D.\n", idx); }

/**
 * @brief Returns the largest number from a and b.
 */ 
double max(double a, double b) { return (a > b) ? a : b; }

/**
 * @brief Returns the largest number from a, b and c.
 */ 
double max3(double a, double b, double c) { return max(max(a, b), c); }

/**
 * @brief Returns true if the first n slots of the cells array
 * are valid (true). Otherwise, returns false.
 * 
 * The last process to come here will have this be valid.
 */ 
bool validate(bool *cells, int n)
{
    for (int i = 0; i < n; i++){
        if (!cells[i]) return false;
    }
    return true;
}

/**
 * @brief Computes and outputs the final results - determinant and
 * largest number - provided all child processes have completed.
 */ 
void output_result(st_shmem* shmem)
{
    double determinant = shmem->D[0] + shmem->D[1] + shmem->D[2];
    double largest_val = max3(shmem->L[0], shmem->L[1], shmem->L[2]);

    printf("\nThe determinant is D=%0.4f, and the largest number in the matrix is L=%0.4f\n",
        determinant, largest_val);
}

/**
 * @brief Work unit for each child after computing the cofactor. Includes
 * computing the largest number in the working row and outputing the final
 * result if last process to complete.
 */ 
void do_work(int idx, st_shmem* shmem)
{
    shmem->L[idx] = max3(shmem->M[idx][0], shmem->M[idx][1], shmem->M[idx][2]);
    shmem->valid[idx] = 1;

    if (validate(shmem->valid, 3)) output_result(shmem);
}

int main()
{
    pid_t pids[3];  // Process IDs
    int idx;        // Stores process index of D
    
    struct timeval initial, final;  // Timing start to completion

    /* Shared memory */
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    int shmid;
    ASSERT((shmid = shmget(IPC_PRIVATE, sizeof(st_shmem), 0666 | IPC_CREAT)) != -1, "Create shared memory.");   // Create the shared memory
    ATTACH_MEM(shmid, PARENT);  // Attach shared memory

    st_shmem* st_shared = (st_shmem *)shared_memory; // Parent and children share same virtual address space, so this address is consistent for all processes.
    double test_mat[9] = { 20, 20, 50, 
                           10,  6, 70, 
                           40,  3,  2 };

    // Initialize the shared memory with nil values and test matrix.
    for (int i = 0; i < M_SIZE * M_SIZE; i++) 
    {
        if (i < 3) 
        {
            st_shared->D[i] = 0;
            st_shared->L[i] = 0;
            st_shared->valid[i] = 0;
        }
        st_shared->M[i / 3][i % 3] = test_mat[i];
    }

    /* Begin Forking */
    gettimeofday(&initial, NULL);   // Start timer

    ASSERT((pids[0] = fork()) > -1, "Fork first child.");   // Fork failed

    if (pids[0] > 0)
    {
        ASSERT((pids[1] = fork()) > -1, "Fork second child.");  // Fork failed
        if (pids[1] > 0)
        {
            ASSERT((pids[2] = fork()) > -1, "Fork third child.");   // Fork failed
            if (pids[2] > 0)
            {
                // Parent
                //printf("[Parent] PID: %d, PPID: %d\n", getpid(), getppid());
                wait_n_children(3); // Wait for three children to exit

                // Destroy the shared memory
                ASSERT(shmctl(shmid, IPC_RMID, 0) != -1, "Cleanup share memory.");

                gettimeofday(&final, NULL); // End timer

                // Print timer statistics
                printf("\nStart Time: %lf sec from Epoch (1970-01-01 00:00:00 +0000 (UTC))\n", initial.tv_sec + (double)initial.tv_usec/MICRO_SEC_IN_SEC);
                printf("End Time: %lf sec from Epoch (1970-01-01 00:00:00 +0000 (UTC))\n", final.tv_sec + (double)final.tv_usec/MICRO_SEC_IN_SEC);
                printf("Forking Elapsed Time: %ld micro sec \n\n", ((final.tv_sec * MICRO_SEC_IN_SEC + final.tv_usec) - (initial.tv_sec * MICRO_SEC_IN_SEC + initial.tv_usec)));

            }
            else
            {
                // P3
                //printf("[P3] PID: %d, PPID: %d\n", getpid(), getppid());
                ATTACH_MEM(shmid, P3);  // Attach memory
                
                idx = 2;
                print_status(idx); 
                st_shared->D[idx] = st_shared->M[0][idx] * (    // Calculate cofactor
                    st_shared->M[1][0] * st_shared->M[2][1] -
                    st_shared->M[1][1] * st_shared->M[2][0] 
                );
                do_work(idx, st_shared);                        // Calculate largest

                DETTACH_MEM(st_shared, P3);                     // Dettach memory
            }
        }
        else 
        {
            // P2
            //printf("[P2] PID: %d, PPID: %d\n", getpid(), getppid());       
            ATTACH_MEM(shmid, P2);

            idx = 1;
            print_status(idx);
            st_shared->D[idx] = -st_shared->M[0][idx] * (   // Calculate cofactor
                st_shared->M[1][0] * st_shared->M[2][2] -
                st_shared->M[1][2] * st_shared->M[2][0] 
            );
            do_work(idx, st_shared);                        // Calculate largest

            DETTACH_MEM(st_shared, P2);                     // Dettach memory
        }

    }
    else 
    {
        // P1
        //printf("[P1] PID: %d, PPID: %d\n", getpid(), getppid());
        ATTACH_MEM(shmid, P1);

        idx = 0;
        print_status(idx);
        st_shared->D[idx] = st_shared->M[0][idx] * (        // Calculate cofactor
            st_shared->M[1][1] * st_shared->M[2][2] -
            st_shared->M[1][2] * st_shared->M[2][1] 
        );
        do_work(idx, st_shared);                            // Calculate largest

        DETTACH_MEM(st_shared, P1);                         // Dettach memory
    }

    exit(EXIT_SUCCESS);
}
