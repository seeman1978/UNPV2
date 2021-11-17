//
// Created by wq on 2021/11/15.
//
#include "unpipc.h"

void server(int readfd, int writefd){
    int fd;
    size_t n;
    char buff[MAXLINE+1];
    if (n= Read(readfd, buff, MAXLINE); n == 0){
        err_quit("end-of-line while reading pathname");
    }
    buff[n] = '\n';
    if (fd = open(buff, O_RDONLY); fd < 0){
        snprintf(buff+n, sizeof(buff)-n, ": can't open , %s\n", strerror(errno));
        n = strlen(buff);
        Write(writefd, buff, n);
    }
    else{
        while ((n=Read(fd, buff, MAXLINE)) > 0){
            Write(writefd, buff, n);
        }
        Close(fd);
    }
}