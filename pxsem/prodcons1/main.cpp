#include <iostream>
#include <semaphore.h>
#include "unpipc.h"
#define NBUFF 5
#define SEM_MUTEX "/mutex"
#define SEM_NEMPTY "/nempty"
#define SEM_NSTORED "/nstored"

int nitems;
struct {
    int buff[NBUFF];
    sem_t *mutex, *nempty, *nstored;
} shared;

void* produce(void* arg){
    for (int i = 0; i < nitems; ++i) {
        sem_wait(shared.nempty);///wait for at least one empty slot
        sem_wait(shared.mutex);
        shared.buff[i%NBUFF] = i;//store i into circular buffer
        sem_post(shared.mutex);
        sem_post(shared.nstored);
    }
    return nullptr;
}

void* consume(void* arg){
    for (int i = 0; i < nitems; ++i) {
        sem_wait(shared.nstored);///wait for at least one empty slot
        sem_wait(shared.mutex);
        if(shared.buff[i%NBUFF] != i){
            printf("buff[%d]=%d\n", i, shared.buff[i%NBUFF]);
        }
        sem_post(shared.mutex);
        sem_post(shared.nempty);
    }
    return nullptr;
}

int main(int argc, char** argv) {
    pthread_t tid_produce, tid_consume;
    if (argc != 2){
        err_quit("usage: prodconns1 <#items>");
    }
    nitems = atoi(argv[1]);
    shared.mutex = sem_open(SEM_MUTEX, O_CREAT, 0644, 1);
    if (shared.mutex == SEM_FAILED){
        perror("Failed to open semphore for mutex");
        exit(-1);
    }

    shared.nempty = sem_open(SEM_NEMPTY, O_CREAT, FILE_MODE, NBUFF);
    if (shared.nempty == SEM_FAILED){
        perror("Failed to open semphore for nempty");
        exit(-1);
    }
    shared.nstored = sem_open(SEM_NSTORED, O_CREAT, FILE_MODE, 0);
    if (shared.nstored == SEM_FAILED){
        perror("Failed to open semphore for nstored");
        exit(-1);
    }
    ///create one producer and one consumer thread
    Set_concurrency(2);
    Pthread_create(&tid_produce, nullptr, produce, nullptr);
    Pthread_create(&tid_consume, nullptr, consume, nullptr);
    //wait for the two threads
    Pthread_join(tid_produce, nullptr);
    Pthread_join(tid_consume, nullptr);
    //remove the semaphores
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_NEMPTY);
    sem_unlink(SEM_NSTORED);
    return 0;
}
