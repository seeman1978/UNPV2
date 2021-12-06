#include <iostream>
#include <sys/mman.h>
#include "unpipc.h"
struct shmstruct{
    int count;
};
sem_t *mutex;
int main(int argc, char** argv) {
    int fd, nloop;
    pid_t pid;
    struct shmstruct *ptr;
    if (argc != 4){
        err_quit("usage: client1 <shmname> <semname> <#nloop>");
    }
    nloop = atoi(argv[3]);
    fd = Shm_open(argv[1], O_RDWR, FILE_MODE);
    ptr = static_cast<shmstruct *>(Mmap(nullptr, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    Close(fd);
    mutex = Sem_open(argv[2], 0);
    pid = getpid();
    for (int i = 0; i < nloop; ++i) {
        Sem_wait(mutex);
        printf("pid %ld:%d\n", (long)pid, ptr->count++);
        Sem_post(mutex);
    }
    return 0;
}
