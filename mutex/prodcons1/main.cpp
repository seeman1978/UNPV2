#include <iostream>
#include "unpipc.h"
constexpr auto MAXNITEMS = 1000000;
constexpr auto MAXNTHREADS = 100;

int nitems;///readonly by producer and consumer
struct {
    pthread_mutex_t mutex;
    int buff[MAXNITEMS];
    int nput;
    int nval;
} shared = {
        PTHREAD_MUTEX_INITIALIZER
};

void* produce(void *arg){
    for (;;){
        Pthread_mutex_lock(&shared.mutex);
        if (shared.nput >= nitems){
            Pthread_mutex_unlock(&shared.mutex);
            return nullptr; //array is full
        }
        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;
        Pthread_mutex_unlock(&shared.mutex);
        *((int*)arg) +=1;
    }
}

void* consume(void *arg){
    for (int i = 0; i < nitems; ++i) {
        if (shared.buff[i] != i){
            printf("buff[%d] = %d\n", i, shared.buff[i]);
        }
    }
    return nullptr;
}

int main(int argc, char** argv) {
    int nthreads, count[MAXNTHREADS];
    pthread_t tid_produce[MAXNTHREADS], tid_consume;
    if (argc != 3){
        err_quit("usages: prodcons1 <#items> <#threads>");
    }
    nitems = min(atoi(argv[1]), MAXNITEMS);
    nthreads = min(atoi(argv[2]), MAXNTHREADS);
    Set_concurrency(nthreads);
    ///start all the producer threads
    for (int i = 0; i < nthreads; ++i) {
        count[i] = 0;
        Pthread_create(&tid_produce[i], nullptr, produce, &count[i]);
    }
    ///wait for all the producer threads
    for (int i = 0; i < nthreads; ++i) {
        Pthread_join(tid_produce[i], nullptr);
        printf("count[%d]=%d\n", i, count[i]);
    }
    //start, then wait for the consumer thread
    Pthread_create(&tid_consume, nullptr, consume, nullptr);
    Pthread_join(tid_consume, nullptr);
    return 0;
}
