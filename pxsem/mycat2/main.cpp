#include <iostream>
#include "unpipc.h"
#define NBUFF 8
struct{
    struct{
        char data[BUFFSIZE];
        ssize_t n;
    } buff[NBUFF];
    sem_t mutex, nempty, nstored;
} shared;
int fd;

void* produce(void* arg){
    for (int i=0;;){
        Sem_wait(&shared.nempty);
        Sem_wait(&shared.mutex);
        Sem_post(&shared.mutex);
        shared.buff[i].n = Read(fd, shared.buff[i].data, BUFFSIZE);
        if (shared.buff[i].n == 0){
            Sem_post(&shared.nstored);  //let consumers terminate
            return nullptr;
        }
        if (++i >= NBUFF){
            i = 0;
        }
        Sem_post(&shared.nstored);
    }
}

void* consume(void *arg){
    int i;
    for (i=0;;) {
        Sem_wait(&shared.nstored);
        Sem_wait(&shared.mutex);
        Sem_post(&shared.mutex);
        if (shared.buff[i].n == 0){
            return nullptr;
        }
        Write(STDOUT_FILENO, shared.buff[i].data, shared.buff[i].n);
        if (++i >= NBUFF){
            i = 0;
        }
        Sem_post(&shared.nempty);
    }
}

int main(int argc, char** argv) {
    pthread_t tid_produce, tid_consume;
    if (argc != 2){
        err_quit("usages: mycat2 <pathname>");
    }
    fd = Open(argv[1], O_RDONLY);
    Sem_init(&shared.mutex, 0, 1);
    Sem_init(&shared.nempty, 0, NBUFF);
    Sem_init(&shared.nstored, 0, 0);
    Set_concurrency(2);
    Pthread_create(&tid_produce, nullptr, produce, nullptr);
    Pthread_create(&tid_consume, nullptr, consume, nullptr);

    Pthread_join(tid_produce, nullptr);
    Pthread_join(tid_consume, nullptr);

    Sem_destroy(&shared.mutex);
    Sem_destroy(&shared.nempty);
    Sem_destroy(&shared.nstored);
    return 0;
}
