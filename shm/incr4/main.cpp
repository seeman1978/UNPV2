#include <iostream>
#include "unpipc.h"
#define SEM_NAME "mysem"
#include <sys/mman.h>
int main(int argc, char** argv) {
    int nloop;
    int *ptr;
    sem_t *mutex;
    if (argc != 2){
        err_quit("usage: incr2 <#loops>");
    }
    nloop = atoi(argv[1]);

    ptr = static_cast<int *>(Mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0));
    mutex = Sem_open(SEM_NAME, O_CREAT|O_EXCL, FILE_MODE, 1);
    Sem_unlink(SEM_NAME);
    setbuf(stdout, nullptr);    ///set stdout is unbuffered
    if (Fork() == 0){   ///child
        for (int i = 0; i < nloop; ++i) {
            Sem_wait(mutex);
            printf("child : %d\n", (*ptr)++);
            Sem_post(mutex);
        }
        exit(0);
    }
    //parent
    for (int i = 0; i < nloop; ++i) {
        Sem_wait(mutex);
        printf("parent : %d\n", (*ptr)++);
        Sem_post(mutex);
    }
    return 0;
}
