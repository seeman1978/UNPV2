#include <iostream>
#include <sys/mman.h>
#include "unpipc.h"
#include "cliserv2.h"
int main(int argc, char** argv) {
    int fd, nloop, nusec;
    pid_t pid;
    char mesg[MESGSIZE];
    long offset;
    struct shmstruct *ptr;
    if (argc != 4){
        err_quit("usage: client2 <shmname> <#loops> <#usec>");
    }
    nloop = atoi(argv[2]);
    nusec = atoi(argv[3]);

    fd = shm_open(argv[1], O_RDWR, FILE_MODE);
    ptr = static_cast<shmstruct *>(Mmap(nullptr, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));\
    Close(fd);
    pid = getpid();
    //initial the array of offsets
    for (int i = 0; i < nloop; ++i) {
        Sleep_us(nusec);
        snprintf(mesg, MESGSIZE, "pid %ld: message %d", (long)pid, i);
        if (sem_trywait(&ptr->nempty) == -1){
            if (errno == EAGAIN){
                Sem_wait(&ptr->noverflowmutex);
                ptr->noverflow++;
                Sem_post(&ptr->noverflowmutex);
                continue;
            }
            else{
                err_sys("sem_trywait error");
            }
        }
        Sem_wait(&ptr->mutex);
        offset = ptr->msgoff[ptr->nput];
        if (++(ptr->nput) >= NMESG){
            ptr->nput = 0;///circular buffer
        }
        Sem_post(&ptr->mutex);
        strcpy(&ptr->msgdata[offset], mesg);
        Sem_post(&ptr->nstored);
    }

    return 0;
}
