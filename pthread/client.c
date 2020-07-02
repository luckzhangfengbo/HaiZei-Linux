






#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int sockfd, port;
    struct sockaddr_in server;
    if (argc != 3) { // ./a.out IP port
        fprintf(stderr, "Usage: %s ip port\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[2]);

    server.sin_family = AF_INET;
    server.sin_port = htons(port); //将整型变量从主机字节顺序转变成网络字节顺序， 就是整数在地址空间存储方式变为高位字节存放在内存的低地址处
        server.sin_addr.s_addr = inet_addr(argv[1]); //inet_addr函数转换网络主机地址（如192.168.1.10)为网络字节序二进制值，如果参数char *cp无效，函数返回-1(INADDR_NONE)

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    printf("Socket create.\n");
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        exit(1);
    }
    if (send(sockfd, "zhangzhiguo", sizeof("zhangzhiguo"), 0) < 0) { //1)指定发送端套接字描述符 2)存放要发送数据的缓冲区 3)实际要改善的数据的字节数 4)flags：一般设置为0
        perror("send");
        exit(1);
    }
    close(sockfd);
    return 0;
}
