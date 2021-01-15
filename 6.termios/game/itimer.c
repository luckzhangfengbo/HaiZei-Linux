#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
/*该结构体要配合函数setitimer使用。函数原型为

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);

其中which参数表示类型，可选的值有：

ITIMER_REAL：以系统真实的时间来计算，它送出SIGALRM信号。

ITIMER_VIRTUAL：以该进程在用户态下花费的时间来计算，它送出SIGVTALRM信号。
ITIMER_PROF：以该进程在用户态下和内核态下所费的时间来计算，它送出SIGPROF信号。
old_value的值一般为NULL。
settimer工作机制是，先对it_value倒计时，当it_value为零时触发信号，然后重置为it_interval，继续对it_value倒计时，一直这样循环下去。
*/
void print() {
    printf("Recv a signal!\n");
}

int main() {
    struct itimerval itm;
    signal(14, print);
    itm.it_interval.tv_sec = 1;
    itm.it_interval.tv_usec = 0;
    itm.it_value.tv_sec = 3;
    itm.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itm, NULL);
    while (1) {
        sleep(10); //模仿I/O操作
        printf("After Sleep!\n");
    }
    return 0;
}

