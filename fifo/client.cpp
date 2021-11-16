//
// Created by wq on 2021/11/15.
//

#include "unpipc.h"

void client(int readfd, int writefd){
    size_t len;
    ssize_t n;
    char buff[MAXLINE];
    ///read pathname
    Fgets(buff, MAXLINE, stdin);
    len = strlen(buff);    ///fgets guarantees null byte at end
    if (buff[len-1] == '\n'){
        len--;
    }
    Write(writefd, buff, len);

    while((n = Read(readfd, buff, MAXLINE)) > 0){
        Write(STDOUT_FILENO, buff, n);
    }
}