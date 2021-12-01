#include <iostream>
#include <semaphore.h>
#include "unpipc.h"

#define NBUFF 2
int nitems;
struct {
    int buff[NBUFF];
    sem_t mutex, nempty, nstored;
} shared;

void* produce(void* arg){
    for (int i = 0; i < nitems; ++i) {
        Sem_wait(&shared.nempty);///wait for at least one empty slot
        Sem_wait(&shared.mutex);
        shared.buff[i%NBUFF] = i;//store i into circular buffer
        Sem_post(&shared.mutex);
        Sem_post(&shared.nstored);
    }
    return nullptr;
}

void* consume(void* arg){
    for (int i = 0; i < nitems; ++i) {
        Sem_wait(&shared.nstored);///wait for at least one empty slot
        sem_wait(&shared.mutex);
        if(shared.buff[i%NBUFF] != i){
            printf("buff[%d]=%d\n", i, shared.buff[i%NBUFF]);
        }
        Sem_post(&shared.mutex);
        Sem_post(&shared.nempty);
    }
    return nullptr;
}

int main(int argc, char** argv) {
    pthread_t tid_produce, tid_consume;
    if (argc != 2){
        err_quit("usage: prodconns1 <#items>");
    }
    nitems = atoi(argv[1]);
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
