/*************************************************************************
	> File Name: my_cp.cpp
	> Author:zhanghiguo
	> Mail: 
	> Created Time: 2020年03月22日 星期日 14时32分52秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#define BUFSSIZE 512

int main(int argc, char **argv) {
    int fd_in, fd_out;
    char buf[BUFSSIZE + 5] = {0};
    ssize_t nread;
    if(argc != 3) {
        printf("USEAG: %s SOURCEFILE DESTILE", argv[0]);
        exit(1);
    }
    if((fd_in = open(argv[1], O_RDONLY)) == -1) {
        perror(argv[1]);
        exit(1);
    }
    if((fd_out= creat(argv[2], 0644)) == -1) {
        perror(argv[2]);
        exit(1);
    }
    while((nread = read(fd_in, buf, BUFSSIZE)) > 0){
        if(write(fd_out, buf, strlen(buf)) != nread){
            perror("write");
            exit(1);
        }
        memset(buf, 0, BUFSSIZE + 5);
    }
    
    close(fd_in);
    close(fd_out);
    return 0;
}

