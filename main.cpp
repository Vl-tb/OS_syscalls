// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "options_parser.h"
#include "vector"
#include <stdio.h>

int write_buffer (int fd, const char* buffer , ssize_t size){
    ssize_t written_bytes = 0;
    while( written_bytes < size ) {
        ssize_t written_now = write(fd, buffer + written_bytes, size - written_bytes );
        if( written_now == -1){
            if (errno == EINTR)
                continue;
            else{
                return -3;
            }
        }else if (written_now == 0){
            return 0;
        }
        else
            written_bytes += written_now;
    }
    return 0;
}

ssize_t read_buffer (int fd, char* buffer , ssize_t size){
    ssize_t read_bytes = 0;
    while( read_bytes < size ) {
        ssize_t read_now = read(fd, buffer + read_bytes, size - read_bytes );
        if( read_now == -1){
            if (errno == EINTR)
                continue;
            else{
                return -2;
            }
        }else if (read_now == 0){

            return read_bytes;
        }
        else
            read_bytes += read_now;

    }
    return read_bytes;
}

int myopen(const char *pathname) {
    while (true){
        int fd = open(pathname, 0);
        if (fd < 0) {

            if (errno == EINTR) {
                continue;
            }
            else {
                return -1;
            }

        }else {
            return fd;
        }
    }
}

int myclose(int fd){
    while (true){
        int rt = close(fd);
        if (rt < 0) {
            if (errno == EINTR)
                continue;
            else {
                return -1;
            }
        }else {
            return 0;
        }
    }
}


char buf[10240];
char pid_buf[40960];

void mycat(int fd, bool A_flag) {
    ssize_t n;

    while(true) {
        size_t pid_ind = 0;
        n = read_buffer(fd, buf, sizeof(buf));
        ssize_t n_hex = n;
        if (A_flag) {

            for(ssize_t s = 0; s < n; ++s) {
                if ((!isprint(buf[s])) && (!isspace(buf[s]))) {
                    sprintf(pid_buf + pid_ind, "\\x%02X", static_cast<unsigned char >(buf[s]));
                    pid_ind += 4;
                    n_hex += 3;
                }
                else{
                    pid_buf[pid_ind] = buf[s];
                    ++pid_ind;
                }

            }
        }
        if(n == -1){
            write_buffer(2, "cat: read error\n", 16);
            exit(-2);
        } else if (n > 0) {
            if (!A_flag) {
                if (write_buffer(1, buf, n) != 0) {
                    write_buffer(2, "cat: write error\n", 17);
                    exit(-3);
                }
            } else {
                if (write_buffer(1, pid_buf, n_hex) != 0) {
                    write_buffer(2, "cat: write error\n", 17);
                    exit(-3);
                }
            }

        } else {
            return;
        }
    }
}


int main(int argc, char* argv[]) {
    command_line_options_t command_line_options{argc, argv};
    bool A_flag = command_line_options.get_A_flag();
    int fd, i;

    if(argc <= 1){
        return 0;
    }
    std::vector<int> filesd(argc);

    for(i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-A")) {
            continue;
        }
        if((fd = myopen(argv[i])) < 0){
            std::cout << "cat: cannot open " << argv[i] << std::endl;
            return -1;
        }
        else {
            filesd[i] = fd;
        }
    }

    for(i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-A")) {
            continue;
        }
        mycat(filesd[i], A_flag);
        myclose(filesd[i]);
    }

    return 0;
}
