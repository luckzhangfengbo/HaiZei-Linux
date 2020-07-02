#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    pid_t pid;
    for (int i = 0; i < 10; i++) {
        if ((pid = fork())  < 0) {
            printf("eorro");
        }
        if (pid == 0) {
            sleep(60);
            exit(0); //不退出程序的话，将会向父进程那样继续执行fork()函数，会创建很多子进程
            //break;
        }
    }
    return 0;
}
