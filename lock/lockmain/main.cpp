#include <iostream>
#include <csignal>
#include <fcntl.h>
#include "unpipc.h"
#define SEQFILE "seqno" ///file name
void my_lock(int fd){
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;///lock the whole file
    Fcntl(fd, F_SETLKW, &lock);
}
void my_unlock(int fd){
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;//unlock the whole file
    Fcntl(fd, F_SETLK, &lock);
}
int main(int argc, char** argv) {
    int fd;
    long seqno;
    pid_t pid;
    ssize_t n;
    char line[MAXLINE+1];
    pid = getpid();
    fd = Open(SEQFILE, O_RDWR, FILE_MODE);
    for (int i = 0; i < 20; ++i) {
        my_lock(fd);    ///lock the file
        Lseek(fd, 0L, SEEK_SET);///REWIND BEFORE READ
        n = Read(fd, line , MAXLINE);
        line[n]= '\0';
        n = sscanf(line, "%ld\n", &seqno);
        printf("%s: pid = %ld, seq#=%ld\n", argv[0], (long)pid, seqno);
        seqno++;    //increment sequence number
        snprintf(line, sizeof line, "%ld\n", seqno);
        Lseek(fd, 0L, SEEK_SET);
        Write(fd, line, strlen(line));
        my_unlock(fd);
    }
    return 0;
}
