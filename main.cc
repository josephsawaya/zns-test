#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <linux/blkzoned.h>
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/uio.h>

void test_write(int fd, blk_zone_report* hdr){
    int initial_wp = hdr->zones[0].wp * 512;
    int block_size = 1024 * 4;
    int num = 254;
    int buffer_size = block_size * num;
    const char* alphabet = "abcdefghijklmnopqrstuvwxyz";
    iovec* bl = (iovec*) malloc(sizeof(iovec) * 2);
    for(int i = 0; i < 2; i++) {
        char* buf = (char*) memalign(block_size, sizeof(char) * buffer_size);
        for(int i = 0; i < buffer_size; i++){
            buf[i] = alphabet[i % 26];
        }
        bl[i].iov_base = (void*)buf;
        bl[i].iov_len = sizeof(char) * buffer_size;
    } 
    //for(int i = 0; i < num; i++){
        //ssize_t ret = pwrite(fd, buf, block_size, initial_wp + i * block_size);
	//fsync(fd);
        //buf += block_size;
        //std::cout << "WROTE " << ret << " at " << (initial_wp + i * block_size) << std::endl;
    	//if (ret < 0) {
	    //std::cout << strerror(errno) << std::endl;
	    //return
	//}
    //}
    ssize_t ret = pwritev(fd, bl, 2, initial_wp);
    if (ret < 0) {
        std::cout << strerror(errno) << std::endl;
        return;
    }

}

blk_zone_report* get_zone_report(int fd){ 
    unsigned long long start_sector = 0;
    blk_zone_report *hdr;
    size_t hdr_len;
    int nr_zones;
    int ret;

    ret = ioctl(fd, BLKGETNRZONES, &nr_zones);

    hdr_len = sizeof(blk_zone_report) + nr_zones * sizeof(blk_zone);
    hdr = (blk_zone_report* )malloc(hdr_len);
    if (!hdr) {
        std::cout << "ERROR MALLOC" << std::endl;
        return nullptr;
    }

    while (1) {
        hdr->sector = start_sector;
        hdr->nr_zones = nr_zones;

        ret = ioctl(fd, BLKREPORTZONE, hdr);

        if (ret) {
            std::cout << "ERROR REPORTING ZONE" << std::endl;
            return nullptr;
        }

        if(!hdr->nr_zones) {
            break;
        }

        start_sector = hdr->zones[hdr->nr_zones-1].start + hdr->zones[hdr->nr_zones -1].len;
    }

    return hdr;
}

int main (int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "not enough args" << std::endl;
        return 1;
    }
    char* dev_name = argv[1];
    int fd = open(argv[1], O_RDWR | O_DIRECT);
    uint32_t zone_size;
    int ret;
    ret = ioctl(fd, BLKGETZONESZ, &zone_size);
    if (ret) {
        std::cout << "ERROR" << std::endl;
        return 1;
    }
    std::cout << "Size of one zone: " << zone_size << std::endl;
    blk_zone_report* hdr = get_zone_report(fd);
    test_write(fd, hdr);
    free(hdr);
}