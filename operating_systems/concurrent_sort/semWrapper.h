#include <sys/sem.h>
#include <sys/types.h>

/**
 * @brief Creates num_sem semaphores associated
 * with key. 
 * @param key key_t, the semaphore key.
 * @param num_sem int, the number of semaphores to create.
 * @returns The semid on success, -1 otherwise.
 */
int sem_create(key_t key, int num_sem);

/**
 * @brief Deletes the semaphore associated
 * with semid.
 * @param semid int, the semaphore id.
 * @returns >0 on success, -1 otherwise.
 */ 
int sem_delete(int semid);

/**
 * @brief Sets the value of the sem_num semaphore
 * associated with semid.
 * @param semid int, the semaphore id.
 * @param sem_num int, the semaphore index.
 * @param value int, the value to set the sem to.
 * @returns >0 on success, -1 otherwise.
 */ 
int sem_set(int semid, int sem_num, int value);

/**
 * @brief Signals the sem_num semaphore
 * associated with semid.
 * @param semid int, the semaphore id.
 * @param sem_num int, the semaphore index.
 * @returns 0 on success, -1 otherwise.
 */ 
int sem_signal(int semid, int sem_num);

/**
 * @brief Waits on the sem_num semaphore
 * associated with semid.
 * @param semid int, the semaphore id.
 * @param sem_num int, the semaphore index.
 * @returns 0 on success, -1 otherwise.
 */ 
int sem_wait(int semid, int sem_num);
