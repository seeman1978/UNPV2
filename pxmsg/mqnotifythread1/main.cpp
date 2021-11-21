#include <iostream>
#include <mqueue.h>
#include "unpipc.h"
mqd_t mqd;
struct mq_attr attr;
struct sigevent sigev;
static void notify_thread(union sigval arg){
    ssize_t n;
    void *buff;
    std::cout << "notify thread started\n";
    buff = Malloc(attr.mq_msgsize);
    Mq_notify(mqd, &sigev); ///reregister
    while((n= mq_receive(mqd, static_cast<char*>(buff), attr.mq_msgsize, nullptr))){
        std::cout << "read " << n << " bytes\n";
    }
    if (errno != EAGAIN){
        err_sys("mq_receive error");
    }
    free(buff);
    pthread_exit(nullptr);
}

int main(int argc,char** argv) {
    if (argc != 2){
        err_quit("usage mqnotifythread1 <name>");
    }
    mqd= Mq_open(argv[1], O_RDONLY|O_NONBLOCK);
    Mq_getattr(mqd, &attr);
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = nullptr;
    sigev.sigev_notify_function = notify_thread;
    sigev.sigev_notify_attributes = nullptr;
    Mq_notify(mqd, &sigev);
    for (;;){
        pause();
    }
    return 0;
}
