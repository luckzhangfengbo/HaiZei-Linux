






#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

/*#define MAXTIMES 20

int main() {
    pid_t pid;
    FILE *fp, *fp1;
    int pipefd[2];
    pipe(pipefd);
    //fp = fdopen(pipefd[1], "w");
    //fp1 = fdopen(pipefd[0], "r");
    char buff[1024] = {0};
    for (int i = 0; i < MAXTIMES; i++ ) {
        if ((pid = fork()) < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            break;
        }
    }
    fp = fdopen(pipefd[1], "w");
    fp1 = fdopen(pipefd[0], "r");
    if (pid == 0) {
        //while (1) {
            close(pipefd[0]);
            scanf("%[^\n]s", buff);
            getchar();
            //fprintf(fp, "%s", buff);
            fwrite((void *)buff, 1, sizeof(buff), fp);
            //close(pipefd[1]);
        //}
    } else {
        //while (1) {
            close(pipefd[1]);
            fread((void *)buff, 1, sizeof(buff), fp1);
            //fscanf(fp1, "%s", buff);
            printf("server : %s\n", buff);
            memset(buff, 0, sizeof(buff));
            //close(pipefd[0]);
        //}
            //wait(NULL);
    }
    while (MAXTIMES) {
        wait(NULL);
        MAXTIMES --;
    }
    return 0;
}*/

int main() {
    pid_t pid;
    FILE *fp, *fp1;
    int pipefd[2];
    pipe(pipefd);
    char buff[1024] = {0};
    if ((pid = fork()) < 0) {
        perror("fork");
        return 1;
    }
    fp = fdopen(pipefd[1], "w");
    fp1 = fdopen(pipefd[0], "r");
    if (pid == 0) {
        while (1) {
            scanf("%[^\n]s", buff);
            getchar();
            //printf("strlen(buff) = %d\n", strlen(buff));
            if (strlen(buff) == 0) {
                buff[0] = ' ';
            }
            //printf("%c\n", buff[0]);
            fprintf(fp, "%s\n", buff);
            //fgets(buff, sizeof(buff), fp);
            fflush(fp); //fflush()会强迫将缓冲区内的数据写回参数stream 指定的文件中. 如果参数stream 为NULL,fflush()会将所有打开的文件数据更新.
            memset(buff, 0, sizeof(buff));
        }
    } else {
        while (1) {
            fscanf(fp1, "%s", buff);
            //fputs(buff, fp1);
            printf("server : %s\n", buff);
        }
    }
    return 0;
}
