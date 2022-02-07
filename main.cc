#include "main.h"

int main () {
    unsigned int zone_size;
    int ret;
    ret = ioctl(fd, BLKGETZONESZ, &zone_size);
    if (ret) std::cout << "ERROR\n" << std::endl;
    std::cout << "Size of one zone: " << zone_size << std::endl;
}