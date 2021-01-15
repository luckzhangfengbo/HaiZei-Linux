/*************************************************************************
	> File Name: sender.c
	> Author: suyelu
	> Mail: suyelu@haizeix.com
	> Created Time: Thu 02 Apr 2020 06:39:54 PM CST
 ************************************************************************/

#include "./common/head.h"
#include "./common/tcp_client.h"
#include "./common/common.h"

struct FileMsg{
    long size;//文件大小
    char name[50];//名字
    char buf[4096];//文件属性
};

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s ip port!\n", argv[0]);
        return 1;
    }

    int sockfd, port;//port端口, sockfd是连接用的
    char buff[100] = {0};//读入命令用的
    char name[50] = {0};
    struct FileMsg filemsg;//定义一个结构体

    port = atoi(argv[2]);//将字符转换成整形
    if ((sockfd = socket_connect(argv[1], port)) < 0) {//argv[1]端口
        perror("socket_connect");
        return 1;
    }
    
    while (1) {
        scanf("%[^\n]s", buff);
        getchar();
        if (!strncmp("send ", buff, 5)) {
            strcpy(name, buff + 5);//名字拷贝下来到name中
        } else {
            fprintf(stderr, "invalid command!\n");
            continue;
        }
        //打开文件
        FILE *fp = NULL;
        size_t size;
        if ((fp = fopen(name, "rb")) == NULL) {//以二进制形式打开
            perror("fopen");
            continue;
        }
        /*fseek 找到文件位置   int fseek(FILE *stream, long offset, int whence)
         * 第三个参数 whence 包含 SEEK_SET, SEEK_CUR , SEEK_END
         * SEEK_SET是文件头指针位置，
         * SEEK_CUR是文件当前指针位置
         * SEEK_END是文件末尾指针位置
         */
        //ftail
        fseek(fp, 0L, SEEK_END);//将文件指针移动到末尾
        filemsg.size = ftell(fp);//文件长度
        strcpy(filemsg.name, name);//名字拿到
        fseek(fp, 0L, SEEK_SET);//将文件指针移到头位置
        //读文件
        //fllemsg.buf是清空
        while ((size = fread(filemsg.buf, 1, 4096, fp))) {//将文件读到filemsg中
            send(sockfd, (void *)&filemsg, sizeof(filemsg), 0);//将filemsg发过去
            memset(filemsg.buf, 0, sizeof(filemsg.buf));//将filemsg.buf里面的数据清空
        }
        printf("After Send!\n");
    }
    close(sockfd);
    return 0;
}
