#include <iostream>
#include "unpipc.h"
int main() {
    int fd[2];
    ssize_t n;
    char c;
    pid_t childpid;
    Pipe(fd);///assume a full-duplex pipe
    if (childpid = Fork(); childpid == 0){  ///child
        sleep(3);
        if (n = Read(fd[0], &c, 1); n!=1){
            err_quit("child:read returned %d", n);
        }
        printf("child read %c\n", c);
        Write(fd[0], (void *) "c", 1);
        exit(0);
    }
    ///parent
    Write(fd[1], (void *) "p", 1);
    if (n = Read(fd[1], &c, 1); n != 1){
        err_quit("parent : read returned %d", n);
    }
    printf("parent read %c\n", c);
    return 0;
}
