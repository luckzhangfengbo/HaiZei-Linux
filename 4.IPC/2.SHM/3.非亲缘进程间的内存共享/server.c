/*************************************************************************
	> File Name: server.c
	> Author:fangsong
	> Mail: 
	> Created Time: 2020年04月05日 星期日 00时59分21秒
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

void print() {
    printf("<server> : %s\n", share_memory->buff);
}

int main() {
    int shmid;
    key_t key = ftok(".", 198);
    if((shmid = shmget(key, sizeof(struct Shm), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }
    printf("After shmget!\n");
    if((share_memory = shmat(shmid, NULL, 0)) == (struct Shm *)-1) {
        perror("shmat");
        return 1;
    }
    share_memory->pid = getpid();
    printf("share_memory->pid=%d\n",share_memory->pid);
    printf("After shmat!\n");
    while(1) {
        signal(SIGUSR2, print);
        pause();
    }
    return 0;
}

