






#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

static pid_t *childpid = NULL; //存储子进程的pid，让父进程知道自己的孩子是谁
static int max; //最多能打开文件的数目

FILE *m_popen(const char *command, const char *type) {
    FILE *fp;
    int pfd[2];
    pid_t pid;

    /*判断第二个参数是否合法*/
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
        errno = EINVAL;
        return NULL;
    }
    
    /*第一次执行时，需要给数组开辟空间*/
    if (childpid == NULL) {
        max = sysconf(_SC_OPEN_MAX); //用宏定义，兼容性
        if ((childpid = (pid_t *)calloc(max, sizeof(pid_t))) == NULL)
            return NULL;
    }

    /*判断创建管道是否成功*/
    if (pipe(pfd) < 0) {
        return NULL;
    }

    /*判断文件描述符是否越界*/
    if (pfd[0] >= max || pfd[1] >= max) {
        close(pfd[0]);
        close(pfd[1]);
        return NULL;
    }

    if ((pid = fork()) < 0) {
        return NULL;
    }

    /*子进程*/
    if (pid == 0) {
        if (type[0] == 'r') { //参数为'r'，子进程返回值，要给父进程读，所以需要关闭管道的读端
            close(pfd[0]);
            if (pfd[1] != STDOUT_FILENO) {
                dup2(pfd[1], STDOUT_FILENO); //重定向
                close(pfd[1]);
            }
        } else {
            close(pfd[1]);
            if (pfd[0] != STDIN_FILENO) {
                dup2(pfd[0], STDIN_FILENO);
                close(pfd[0]); //上一行代码复制了一个，所以可以把原来的给关闭
            } 
        }
    /*关闭子进程继承而来的以前的管道*/
    for (int i = 0; i <= max; i++)
        if (childpid[i] > 0) //存储子进程pid，下标为文字描述符
            close(i);

    execl("/bin/sh", "sh", "-c", command, NULL);
    exit(0);
}
    
//不在此wait是因为，管道里面的文件需要边写边流出，不能等所有东西都写进去了然后再流出

    /*父进程*/
    if (type[0] == 'r') {
        close(pfd[1]);
        if ((fp = fdopen(pfd[0], type)) == NULL)
            return NULL;
    } else {
        close((fp = fdopen(pfd[1], type)) == NULL);
            return NULL;
    }
    
    childpid[fileno(fp)] = pid; //将管道的文件描述符和子进程对应起来
    return fp;
}

int m_pclose(FILE *fp) {
    int status, fd, pid;
    if (childpid == NULL) {
        errno = EINVAL;
        return -1;
    }
    fd = fileno(fp);
    if (fd >= max) {
        errno = EINVAL;
        return -1;
    }
    pid = childpid[fd];
    if (pid == 0) { //表示没有相应的子进程存在，子进程pid=0(只是它自己认为)，父进程中是它真实的pid
        errno = EINVAL;
        return -1;
    }
    childpid[fd] = 0; //这个fd没了 的对应的子进程也没了
    close(fd);
    waitpid(pid, &status, 0);
    return status;    
}

int main() {
    FILE *fp;
    char buff[1024] = {0};
    if ((fp = m_popen("ls -al /etc", "r")) == NULL) {
        perror("m_popen");
        return 1;
    }
    while (fgets(buff, 1024, fp) != NULL) {
        printf("%s\n", buff);
    }
    int status = m_pclose(fp);
    printf("status = %d\n", status);
    return 0;
}
