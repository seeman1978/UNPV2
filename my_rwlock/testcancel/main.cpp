#include <iostream>
#include "unpipc.h"
#include "pthread_rwlock.h"

Pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_t tid1, tid2;
void *thread1(void *arg){
    Pthread_rwlock_rdlock(&rwlock);
    printf("thread1() got a read lock\n");
    sleep(3);
    pthread_cancel(tid2);
    sleep(3);
    Pthread_rwlock_unlock(&rwlock);
    return nullptr;
}

void* thread2(void * arg){
    printf("thread2 trying to obtain a write lock\n");
    Pthread_rwlock_wrlock(&rwlock);
    printf("thread2() got a write lock\n");
    sleep(1);
    Pthread_rwlock_unlock(&rwlock);
    return nullptr;
}

int main() {
    void * status;
    Set_concurrency(2);
    Pthread_create(&tid1, nullptr, thread1, nullptr);
    sleep(1);
    Pthread_create(&tid2, nullptr, thread2, nullptr);
    Pthread_join(tid2, &status);
    if (status != PTHREAD_CANCELED){
        printf("thread2 status = %p\n", status);
    }
    Pthread_join(tid1, &status);
    if (status != nullptr){
        printf("thread1 status = %p\n", status);
    }
    printf("rw_refcount = %d, rw_nwaitreaders = %d, rw_nwaitwriters = %d\n",
           rwlock.rw_refcount, rwlock.rw_nwaitreaders, rwlock.rw_nwaitwriters);
    Pthread_rwlock_destroy(&rwlock);
    return 0;
}
