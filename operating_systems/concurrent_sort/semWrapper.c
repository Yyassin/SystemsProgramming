#include "semun.h"
#include "semWrapper.h"

int sem_create(key_t key, int num_sem)
{
    return semget(key, num_sem, 0666 | IPC_CREAT);
}

int sem_delete(int semid)
{
    union semun sem_union;

    return semctl(semid, 0, IPC_RMID, sem_union);
}

int sem_set(int semid, int sem_num, int value)
{
    union semun sem_union;
    sem_union.val = value;

    return semctl(semid, sem_num, SETVAL, sem_union);
}

int sem_wait(int semid, int sem_num)
{
    struct sembuf sem_b;
    sem_b.sem_num = sem_num;
    sem_b.sem_op = -1;          // Wait by decrementing
    sem_b.sem_flg = SEM_UNDO;   // Undo to avoid locks in exceptional termination

    return semop(semid, &sem_b, 1);
}

int sem_signal(int semid, int sem_num)
{
    struct sembuf sem_b;
    sem_b.sem_num = sem_num;    
    sem_b.sem_op = 1;           // Signal by incrementing
    sem_b.sem_flg = SEM_UNDO;   // Undo to avoid locks in exceptional termination

    return semop(semid, &sem_b, 1);
}

int semGetValue(int semid, int semaphore_number)
{
  union semun arg;

  return semctl(semid, semaphore_number, GETVAL, arg);
}
