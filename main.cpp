// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "options_parser.h"
#include "vector"

int write_buffer (int fd, const char* buffer , ssize_t size , int* status ){
    ssize_t written_bytes = 0;
    while( written_bytes < size ) {
        ssize_t written_now = write(fd, buffer + written_bytes, size - written_bytes );
        if( written_now == -1){
            if (errno == EINTR)
            continue;
            else{
                *status = errno;
                return -1;
            }
        }else if (written_now == 0){
            return 0;
        }
        else
            written_bytes += written_now;
    }
    return 0;
}

int read_buffer (int fd, char* buffer , ssize_t size , int* status ){
    ssize_t read_bytes = 0;
    while( read_bytes < size ) {
        printf("1: %zd\n", read_bytes);
        ssize_t read_now = read(fd, buffer + read_bytes, size - read_bytes );
        printf("2: %zd\n", read_now);
        if( read_now == -1){
            if (errno == EINTR)
                continue;
            else{
                // check if status == 0
                *status = errno;
                return -1;
            }
        }else if (read_now == 0){
            return 0;
        }
        else
            read_bytes += read_now;

    }

    return read_bytes;
}

int open(const char *pathname, int* status) {
    while (true){
        int fd = open(pathname, 0);
        if (fd < 0) {
            if (errno == EINTR)
                continue;
            else {
                *status = errno;
                return -1;
            }
        }else {
            return fd;
        }
    }
}

int close(int fd, int* status){
    while (true){
        int rt = close(fd);
        if (rt < 0) {
            if (errno == EINTR)
                continue;
            else {
                *status = errno;
                return -1;
            }
        }else {
            return 0;
        }
    }
}


char buf[512];
int* status;

void cat(int fd) {
    int n;

    while(true) {
//    for(int r = 0; r < 3; r++) {
        memset(buf, 0, 512); // empty buffer
        n = read_buffer(fd, buf, sizeof(buf), status);
        printf("\nHere. n = %d\n", n);

        if(n == -1){
            write_buffer(2, "cat: read error\n", 16, status);
            exit(2);
        } else if (n == 0) {
            if (write_buffer(1, buf, sizeof(buf), status) != 0) {
                write_buffer(2, "cat: write error\n", 17, status);
            }
            return;
        } else {
            if (write_buffer(1, buf, sizeof(buf), status) != 0) {
                write_buffer(2, "cat: write error\n", 17, status);
            }
        }
    }
    return;
}


int main(int argc, char* argv[]) {
    command_line_options_t command_line_options{argc, argv};
    std::cout << "A flag value: " << command_line_options.get_A_flag() << std::endl;
    int fd, i;

    if(argc <= 1){
        return 0;
    }
    std::vector<int> filesd(argc);

    for(i = 1; i < argc; i++) {
        if((fd = open(argv[i], status)) < 0){
            write_buffer(2, reinterpret_cast<const char *>(printf("cat: cannot open %s\n", argv[i])), 50, status);
            return 1;
        }
        else {
            printf("fd=%d\n", fd);
            filesd[i] = fd;
        }
    }

    for(i = 1; i < argc; i++) {
        cat(filesd[i]);
        close(filesd[i], status);
    }

    return 0;
}
