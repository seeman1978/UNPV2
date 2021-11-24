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

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
