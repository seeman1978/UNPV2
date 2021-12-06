#include <sys/mman.h>
#include "unpipc.h"
int main(int argc, char** argv) {
    int fd;
    struct stat stat{};
    unsigned char *ptr, c;
    if (argc != 2){
        err_quit("usage: shmread <name>");
    }
    fd = Shm_open(argv[1], O_RDWR, FILE_MODE);
    Fstat(fd, &stat);
    ptr = static_cast<unsigned char *>(Mmap(nullptr, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    Close(fd);
    for (int i = 0; i < stat.st_size; ++i) {
        if (c=*ptr++; c!=(i%256)){
            err_ret("ptr[%d] = %d", i, c);
        }
    }
    return 0;
}
