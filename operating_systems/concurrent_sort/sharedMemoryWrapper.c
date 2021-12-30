#include "SharedMemoryWrapper.h"

int shmem_create(key_t key, int size)
{
	return shmget(key, size, IPC_CREAT | 0666);
}

void* shmem_attach(int shmid)
{
    return shmat(shmid, 0, 0);
}

int shmem_dettach(const void* shmaddr)
{
    return shmdt(shmaddr);
}

int shmem_delete(int shmid)
{
    return shmctl(shmid, IPC_RMID, 0);
}
