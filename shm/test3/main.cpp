#include <iostream>
#include <sys/mman.h>
#include "unpipc.h"
int main(int argc, char** argv) {
    int fd1, fd2, *ptr1, *ptr2;
    pid_t childpid;
    struct stat stat;
    if (argc != 2){
        err_quit("usage: test3 <name>");
    }
    shm_unlink(argv[1]);
    fd1 = Shm_open(argv[1], O_RDWR|O_CREAT|O_EXCL, FILE_MODE);
    Ftruncate(fd1, sizeof(int));
    fd2 = Open("/etc/motd", O_RDONLY);
    Fstat(fd2, &stat);
    if (childpid = Fork(); childpid == 0){///child
        //ptr2 = static_cast<int *>(Mmap(nullptr, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0));
        ptr1 = static_cast<int *>(Mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0));
        printf("child: shm ptr = %p\n", ptr1);
        sleep(5);
        printf("shared memory integer = %d\n", *ptr1);
        exit(0);
    }
    ptr1 = static_cast<int *>(Mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0));
    //ptr2 = static_cast<int *>(Mmap(nullptr, stat.st_size, PROT_READ, MAP_SHARED, fd2, 0));
    printf("parent: shm ptr=%p\n", ptr1);
    *ptr1 = 777;
    Waitpid(childpid, nullptr, 0);
    return 0;
}
