#include "main.h"
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>

int main (int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "not enough args" << std::endl;
        return 1;
    }
    int fd = open(argv[1], O_RDWR);
    unsigned int zone_size;
    int ret;
    ret = ioctl(fd, BLKGETZONESZ, &zone_size);
    if (ret) {
        std::cout << "ERROR\n" << std::endl;
        return 1;
    }
    std::cout << "Size of one zone: " << zone_size << std::endl;
}