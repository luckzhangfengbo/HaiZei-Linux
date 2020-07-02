#include <stdio.h>
#define Q "Do you want another try?"
//目标输入 y/n 不用按回车，直接开始下一步
int getresponse();

int main() {
    int response;
    response = getresponse();
    return response;
}

int getresponse() {
    printf("%s", Q);
    printf("(y/n) : ");
    switch (getchar()) {
        case 'y':
        case 'Y': return 1;
        case 'N':
        case 'n': return 0;
        default: return 0;
    }
}

