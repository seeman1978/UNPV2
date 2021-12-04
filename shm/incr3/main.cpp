#include <iostream>
#include "unpipc.h"
#define SEM_NAME "mysem"
#include <sys/mman.h>

struct shared{
    sem_t mutex;
    int count;
} shared;

int main(int argc, char** argv) {
    int fd, nloop;
    struct shared *ptr;
    sem_t *mutex;
    if (argc != 3){
        err_quit("usage: incr3 <pathname> <#loops>");
    }
    nloop = atoi(argv[2]);

    fd = Open(argv[1], O_RDWR|O_CREAT, FILE_MODE);
    Write(fd, &shared, sizeof(struct shared));
    ptr = static_cast<struct shared *>(Mmap(nullptr, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    Close(fd);
    Sem_init(&ptr->mutex, 1, 1);
    Sem_post(&shared.mutex);
    setbuf(stdout, nullptr);    ///set stdout is unbuffered

    if (Fork() == 0){   ///child
        for (int i = 0; i < nloop; ++i) {
            Sem_wait(&shared.mutex);
            printf("child : %d\n", ptr->count++);
            Sem_post(&shared.mutex);
        }
        exit(0);
    }
    //parent
    for (int i = 0; i < nloop; ++i) {
        Sem_wait(&shared.mutex);
        printf("parent : %d\n", ptr->count++);
        Sem_post(&shared.mutex);
    }
    return 0;
}
