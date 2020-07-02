#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    if (argc != 2) { //./a.out port
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }
    int port, server_listen;
    port = atoi(argv[1]); //将字符串转为整数

    if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) { //创建TCP通道
        perror("socket");
            exit(1);
    }
    
    printf("Socket create.\n");
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port); //将整型变量从主机字节顺序转变成网络字节顺序， 就是整数在地址空间存储方式变为高位字节存放在内存的低地址处
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) { //sockfd=socket函数返回的一个ID-----------*addr=定义的一个struct sockaddr_in结构体的地址需要强制转换--addrlen=addr的sizeof(addr)
        perror("bind");
        exit(1);
    }

    printf("Socket bind.\n");
    if (listen(server_listen, 20) < 0) { //监听来自客户端的tcp socket的连接请求
        perror("server");
        exit(1);
    }

    while(1) {
        int sockfd;
        printf("Socket before accept.\n");
        if ((sockfd = accept(server_listen, NULL, NULL)) < 0) { // accept函数指定服务端去接受客户端的连接，接收后，返回了客户端套接字的标识，且获得了客户端套接字的“地方”（包括客户端IP和端口信息等）
            perror("accept");
            close(sockfd);
            continue;
        }
        pid_t pid; //pid_t 类型在 Linux 环境编程中用于定义进程 ID
        if ((pid = fork()) < 0) { // 克隆，调用一次，返回两次 1）在父进程中，fork返回新创建子进程的进程ID；2）在子进程中，fork返回0  3）如果出现错误，fork返回一个负值
            perror("fork");
            continue;
        }
        if (pid == 0) {
            close(server_listen);
            char name[20] = {0};
            if (recv(sockfd, name, sizeof(name), 0) <= 0) {
                perror("recv");
                close(sockfd);
            }
            printf("Name = %s\n", name);
            close(sockfd);
            exit(0);
        }
    }
    close(server_listen);
    return 0;
}

