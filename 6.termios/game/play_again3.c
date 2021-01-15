#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/select.h>
#include <string.h>

#define Q "Do you want another try?"
#define TIMEOUT 5
int get_response();

void tty_mode(int);
void set_cr_noecho_mode();
void set_nonblock_node();
int get_ok_char();
int get_response();

int main() {
    int response;
    tty_mode(0);
    set_cr_noecho_mode();
    get_response();
    tty_mode(1);
    printf("return val = %d\n", response);
    return response;
}

int get_response() {
    printf("%s (y/n)?", Q);
    fflush(stdout);
    
    struct timeval tv;
    a1:tv.tv_sec = TIMEOUT; //秒
    tv.tv_usec = 0; //毫秒

    fd_set set; //定义一个文件的集合
    FD_ZERO(&set); //清空文件的集合
    FD_SET(0, &set); //放入一个文件（通过文件描述符）   0标准输入， 1标准输出， 2标准错误输出

    int ret = select(1, &set, NULL, NULL, &tv);
    if (ret == 1) {
        if (FD_ISSET(0, &set)) {
            //while (1) {
                int input = tolower(getchar());
                switch (input) {
                    case 'y':
                        return 1;
                    case 'n':
                        return 0;
                    default:
                        goto a1;
                //}
            }
        }
    }
    return 2;
}

int get_ok_char() {//暂时没有
    int c;
    while ((c == getchar()) != EOF && strchr("yYnN", c) == NULL);
    return c;
}

void set_nonblock_node() {
    int termflags;
    termflags = fcntl(0, F_GETFL);
    termflags |= O_NONBLOCK;
    fcntl(0, F_SETFL, termflags);
}

void set_cr_noecho_mode() {
    struct termios ttystate;
    tcgetattr(0, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1;
    ttystate.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &ttystate);
}

void tty_mode (int how) {
    static struct termios orignal_mode;
    if (how == 0) {
        tcgetattr(0, &orignal_mode);
    } else {
        tcsetattr(0, TCSANOW, &orignal_mode);
    }
}
