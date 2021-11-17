#include <iostream>
#include "unpipc.h"

void server(int, int);
constexpr auto SERV_FIFO = "/tmp/fifo.serv";
int main() {
    int readfifo, writefifo, dummyfd, fd;
    char *ptr, buff[MAXLINE+1], fifoname[MAXLINE];
    pid_t pid;
    ssize_t n;
    ///create server's well-known FIFO
    if (mkfifo(SERV_FIFO, FILE_MODE) < 0 && errno != EEXIST){
        err_sys("can't create %s", SERV_FIFO);
    }
    ///open server's well-known FIFO FOR READING
    readfifo = Open(SERV_FIFO, O_RDONLY, 0);
    dummyfd = Open(SERV_FIFO, O_WRONLY, 0); ///only for not end-of-line from read. This is a trick.
    while ((n= Readline(readfifo, buff, MAXLINE)) > 0){
        if (buff[n-1] == '\n'){
            n--;///delete new-line from readline
        }
        buff[n] = '\0';
        if (ptr = strchr(buff, ' '); ptr == nullptr){
            err_msg("bogus request %s", buff);
            continue;
        }
        *ptr++ = 0;///ptr = pathname
        pid = atol(buff);
        snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long)pid);
        if (writefifo = Open(fifoname, O_WRONLY, 0) < 0){
            err_msg("cannot open: %s", fifoname);
            continue;
        }
        if (fd= open(ptr, O_RDONLY); fd < 0){
            snprintf(buff+n, sizeof(buff)-n, "can't open, %s\n", strerror(errno));
            n = strlen(ptr);
            Write(writefifo, ptr, n);
            Close(writefifo);
        }
        else{
            ///open succeeded; copy file to fifo
            while ((n= Read(fd, buff, MAXLINE)) > 0){
                Write(writefifo, buff, n);
            }
            Close(fd);
            Close(writefifo);
        }
    }
    return 0;
}
