#include <stdio.h>
#include <unistd.h>
#include <signal.h>
//sleep计时器
void print() {
    printf("It's time get up\n");
}

int main() {
    signal(14, print); //将内核传来的SIGNALRM进行处理，如果不处理，进程将会结束
    alarm(10); //设置信号传送闹钟，即用来设置信号SIGALRM在经过参数seconds秒数后发送给目前的进程。如果未设置信号SIGALARM的处理函数，那么alarm()默认处理终止进程
    pause(); //pause（）使调用进程（或线程）进入休眠状态，直到传递的信号终止进程或导致调用信号捕获函数
    return 0;
}

