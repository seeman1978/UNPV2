#include <iostream>
#include <semaphore.h>
#include "unpipc.h"
#define NBUFF 10
#define MAXNTHREADS 100
int nitems, nproducers;
struct {
    int buff[NBUFF];
    int nput;
    int nputval;
    sem_t mutext, nempty, nstored;
} shared;

void* produce(void* arg){
    for (;;){
        Sem_wait(&shared.nempty);
        Sem_wait(&shared.mutext);
        if (shared.nput >= nitems){
            Sem_post(&shared.nempty);
            Sem_post(&shared.mutext);
            return nullptr;
        }
        shared.buff[shared.nput%NBUFF] = shared.nputval;
        shared.nput++;
        shared.nputval++;
        Sem_post(&shared.mutext);
        Sem_post(&shared.nstored);
        *((int*)arg) += 1;
    }
}

void* consume(void *arg){
    for (int i = 0; i < nitems; ++i) {
        Sem_wait(&shared.nstored);
        Sem_wait(&shared.mutext);
        if (shared.buff[i%NBUFF] != i){
            printf("error:buff[%d] = %d\n", i, shared.buff[i%NBUFF]);
        }
        Sem_post(&shared.mutext);
        Sem_post(&shared.nstored);
    }
    return nullptr;
}

int main(int argc, char** argv) {
    int count[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS], tid_consume;
    if (argc != 3){
        err_quit("usage: prodcons3 <#items> <#producers>");
    }
    nitems = atoi(argv[1]);
    nproducers = std::min(atoi(argv[2]), MAXNTHREADS);
    Sem_init(&shared.mutext, 0, 1);
    Sem_init(&shared.nempty, 0, NBUFF);
    Sem_init(&shared.nstored, 0, 0);

    Set_concurrency(nproducers+1);
    for (int i = 0; i < nproducers; ++i) {
        count[i] = 0;
        Pthread_create(&tid_produce[i], nullptr, produce, &count[i]);
    }
    Pthread_create(&tid_consume, nullptr, consume, nullptr);
    for (int i = 0; i < nproducers; ++i) {
        Pthread_join(tid_produce[i], nullptr);
        printf("count[%d] = %d\n", i, count[i]);
    }
    Pthread_join(tid_consume, nullptr);
    Sem_destroy(&shared.mutext);
    Sem_destroy(&shared.nempty);
    Sem_destroy(&shared.nstored);
    return 0;
}
