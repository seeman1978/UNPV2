#include <iostream>
#include <semaphore.h>
#include "unpipc.h"
#define NBUFF 10
#define MAXNTHREADS 100
int nitems, nproducers, nconsumers;
struct {
    int buff[NBUFF];
    int nput;
    int nputval;
    int nget;
    int ngetval;
    sem_t mutex, nempty, nstored;
} shared;

void* produce(void* arg){
    for (;;){
        Sem_wait(&shared.nempty);
        Sem_wait(&shared.mutex);
        if (shared.nput >= nitems){
            Sem_post(&shared.nstored);  //let consumers terminate
            Sem_post(&shared.nempty);
            Sem_post(&shared.mutex);
            return nullptr;
        }
        shared.buff[shared.nput%NBUFF] = shared.nputval;
        shared.nput++;
        shared.nputval++;
        Sem_post(&shared.mutex);
        Sem_post(&shared.nstored);
        *((int*)arg) += 1;
    }
}

void* consume(void *arg){
    int i;
    for (;;) {
        Sem_wait(&shared.nstored);
        Sem_wait(&shared.mutex);
        if (shared.nget >= nitems){
            Sem_post(&shared.nstored);
            Sem_post(&shared.mutex);
            return nullptr;
        }
        i = shared.nget % NBUFF;
        if (shared.buff[i%NBUFF] != shared.ngetval){
            printf("error:buff[%d] = %d\n", i, shared.buff[i]);
        }
        shared.nget++;
        shared.ngetval++;
        Sem_post(&shared.mutex);
        Sem_post(&shared.nempty);
        *((int*)arg) += 1;
    }
}

int main(int argc, char** argv) {
    int prodcount[MAXNTHREADS], conscount[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS], tid_consume[MAXNTHREADS];
    if (argc != 4){
        err_quit("usage: prodcons4 <#items> <#producers> <#consumers>");
    }
    nitems = atoi(argv[1]);
    nproducers = std::min(atoi(argv[2]), MAXNTHREADS);
    nconsumers = std::min(atoi(argv[3]), MAXNTHREADS);

    Sem_init(&shared.mutex, 0, 1);
    Sem_init(&shared.nempty, 0, NBUFF);
    Sem_init(&shared.nstored, 0, 0);
    Set_concurrency(nproducers+nconsumers);
    for (int i = 0; i < nproducers; ++i) {
        prodcount[i] = 0;
        Pthread_create(&tid_produce[i], nullptr, produce, &prodcount[i]);
    }
    for (int i = 0; i < nconsumers; ++i) {
        conscount[i] = 0;
        Pthread_create(&tid_consume[i], nullptr, consume, &conscount[i]);
    }
    //wait for all producers and all consumers
    for (int i = 0; i < nproducers; ++i) {
        Pthread_join(tid_produce[i], nullptr);
        printf("producer count[%d] = %d\n", i, prodcount[i]);
    }
    for (int i = 0; i < nconsumers; ++i) {
        Pthread_join(tid_consume[i], nullptr);
        printf("consumer count[%d] = %d\n", i, conscount[i]);
    }
    Sem_destroy(&shared.mutex);
    Sem_destroy(&shared.nempty);
    Sem_destroy(&shared.nstored);
    return 0;
}
