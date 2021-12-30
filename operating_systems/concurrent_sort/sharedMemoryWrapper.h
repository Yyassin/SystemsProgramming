#include <sys/types.h>
#include <sys/shm.h>

/**
 * @brief Creates a shared memory instance 
 * identified by key and of size size.
 * @param key key_t, the shared memory key.
 * @param size int, the sie of the shared memory.
 * @returns The shared memory id if success, -1 otherwise.
 */ 
int shmem_create(key_t key, int size);

/**
 * @brief Attaches the shared memory specified by
 * shmid to the calling process.
 * @param shmid int, the shared memeory id.
 * @returns a pointer to the shared memory block
 * if successful, null otherwise. 
 */ 
void* shmem_attach(int shmid);

/**
 * @brief Detaches the shared memory specified by shmaddr
 * from the calling process.
 * @param shamdder void*, the shared memory pointer.
 * @returns 0 on success, -1 otherwise.
 */ 
int shmem_dettach(const void* shmaddr);

/**
 * @brief Deletes the shared memory specified by shmid.
 * @param shmid int, the shared memory id.
 * @returns 0 on success, -1 otherwise.
 */ 
int shmem_delete(int shmid);
