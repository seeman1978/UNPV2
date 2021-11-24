#include <iostream>
#include "unpipc.h"
constexpr auto MAXNITEMS = 1000000;
constexpr auto MAXNTHREADS = 100;
///global shared by threads
int nitems;
int buff[MAXNITEMS];
struct {
    pthread_mutex_t mutex;
    int nput;   ///next index to store
    int nval;   ///next value to store
} put ={
        PTHREAD_MUTEX_INITIALIZER
};

struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nready;///number ready for consumer
} nready = {
        PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
};

void *produce(void* arg){
    for (;;){
        Pthread_mutex_lock(&put.mutex);
        if (put.nput >= nitems){
            Pthread_mutex_unlock(&put.mutex);
            return nullptr; ///array is full, we are done
        }
        buff[put.nput] = put.nval;
        put.nput++;
        put.nval++;
        Pthread_mutex_unlock(&put.mutex);
        Pthread_mutex_lock(&nready.mutex);
        if (nready.nready == 0){
            Pthread_cond_signal(&nready.cond);
        }
        nready.nready++;
        Pthread_mutex_unlock(&nready.mutex);
        *((int*) arg) += 1;
    }
}

void *consume(void* arg){
    for (int i = 0; i < nitems; ++i) {
        Pthread_mutex_lock(&nready.mutex);
        while (nready.nready == 0){
            Pthread_cond_wait(&nready.cond, &nready.mutex);
        }
        nready.nready--;
        Pthread_mutex_unlock(&nready.mutex);
        if (buff[i] != i){
            printf("buff[%d] = %d\n", i, buff[i]);
        }
    }
    return nullptr;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
