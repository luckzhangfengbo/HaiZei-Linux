






#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/shm.h>

struct Num {
    int now;
    int sum;
};


struct Num *share_memory = NULL;

void do_add(int max, int x, key_t key){
    int shmid;
    while (1) {
    if ((shmid = shmget(key, sizeof(struct Num), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return ;
    }
    printf("After shmget!\n");
    if ((share_memory = (struct Num *)shmat(shmid, NULL, 1)) == NULL) {
        perror("shmat");
        return ;
    }
    printf("After shmat!\n");
    if (share_memory->sum > max) {
        break;
    }
    share_memory->sum += share_memory->now;
    share_memory->now += 1;
    printf("<%d> sum: %d, now: %d\n", x, share_memory->sum, share_memory->now);
    }
}

int main(int argc, char **argv){
    if (argc != 3) {
        fprintf(stderr, "Usage:%s max ins\n", argv[0]);
        return 1;
    }
    int x = 0;
    int max = atoi(argv[1]);
    int ins = atoi(argv[2]);
    pid_t pid;
    key_t key = ftok(".", 198);
    struct Num num;
    num.now = 0;
    num.sum = 0;
    for (int i = 0; i < ins; i++) { //父进程循环创建
        if ((pid = fork()) < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            x = i;
            break;//每个子进程不再去循环创建子进程
        }
    }
    
    if (pid == 0) {
        do_add(max, x, key);
        printf("<%d> exit!\n", x);
        exit(0);
    }
    while(ins) { //每个子进程都需要等
        wait(NULL);
        ins--;
    }
    printf("Ans = %d\n", num.sum);
    return 0;//主函数退出时，会关闭所有打开的文件
}
