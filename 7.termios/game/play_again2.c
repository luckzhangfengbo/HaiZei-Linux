#include <stdio.h>
#include <termios.h>
#define Q "Do you want another try?"

int get_response();

void tty_mode(int);
void set_cr_mode();

int main() {
    int response;
    tty_mode(0); //获取最原始的状态
    set_cr_mode(); //按目前需要进行状态修改
    response = get_response();
    tty_mode(1); //还原回最原始的状态
    printf("return val = %d\n", response);
    return response;
}

int get_response() {
    printf("%s", Q);
    printf("(y/n)");
    switch (getchar()) {
        case 'y':
        case 'Y': return 1;
        case 'N':
        case 'n': return 0;
        default: return 0;
    }
}


void set_cr_mode() {
    struct termios ttystate;
    tcgetattr(0, &ttystate);
    ttystate.c_lflag &= ~ICANON; //取消权威模式
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1; //非权威模式下，读取时得最小字符数
    ttystate.c_cc[VTIME] = 1;//非权威模式读取时的超时时间，输入第一个字符后开始记时
    tcsetattr(0, TCSANOW, &ttystate); //TCSANOW 立即生效
}

void tty_mode (int how) { //how = 1;还原， how = 0获取
    static struct termios orignal_mode;
    if (how == 0) {
        tcgetattr(0, &orignal_mode);
    } else {
        tcsetattr(0, TCSANOW, &orignal_mode);
    }
}
