/*************************************************************************
	> File Name: client.c
	> Author:fangsong
	> Mail: 
	> Created Time: 2020年04月05日 星期日 00时59分54秒
 ************************************************************************/
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<signal.h>
#include<string.h>
struct Shm{
    pid_t pid;
    char buff[1024];
};

struct Shm *share_memory = NULL;

int main() {
    int shmid;
    key_t key = ftok(".", 198);
    if((shmid = shmget(key, sizeof(struct Shm),IPC_CREAT|0666)) < 0) {
        perror("shmget");
        return 1;
    }
    if((share_memory = shmat(shmid, NULL, 0)) == (struct Shm *)-1) {
        perror("shmat");
        return 1;
    }
    printf("%d", share_memory->pid);
    while(1) {
         printf("<client> : ");
         scanf("%[^\n]s",share_memory->buff);
         getchar();
         printf("%d", share_memory->pid);
         if(share_memory->pid == 0) {
             printf("<Error> : 服务端没上线!\n");
         } else {
             kill(share_memory->pid, SIGUSR2);
         }
    }
    return 0;
}

