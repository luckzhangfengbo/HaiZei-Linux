/*************************************************************************
	> File Name: add.c
	> Author:fangsong
	> Mail: 
	> Created Time: 2020年04月05日 星期日 15时21分00秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/file.h>


struct Msg {
	int now;
    int sum;
};
struct Msg *shm = NULL;
char num_file[] = "./.num";
void do_add(int max, int x){
    while (1) {
        FILE *lock = fopen(num_file, "w");
        flock(lock->_fileno, LOCK_EX);
        if(shm->now > max)break;
        shm->sum += shm->now;
        shm->now++;
        printf("<b> : %d : <sum> :%d  <now> : %d\n", x, shm->sum, shm->now);
        flock(lock->_fileno, LOCK_UN);
        fclose(lock);
    }
}

int main(int argc, char **argv) {
	key_t key = ftok(".", 233);
	int shmid;
    if ((shmid = shmget(key, sizeof(struct Msg), IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    return 1;
    }
    if ((shm = shmat(shmid, NULL, 0)) < 0) {
    perror("shmat");
    return 1;
    }
    memset(shm, 0, sizeof(struct Msg));
    shm->now = 0;
        if(argc != 3){
        fprintf(stderr, "Usade: %s max ins\n", argv[0]);
        return 1;
    }
    int x = 0;
    int max = atoi(argv[1]);
    int ins = atoi(argv[2]);
    pid_t pid;

    for(int i = 0; i < ins; i++){
        if((pid = fork()) < 0){
            perror("fork");
            return 1;
        }
        if(pid == 0){
            x = i;
            break;
        }
    }
    if(pid == 0){
        do_add(max, x);
        printf("<%d> exit!\n", x);
        exit(0);
    }
    while(ins){
        wait(NULL);
        ins--;
    }
    printf("Ans = %d\n", shm->sum);
    return 0;
}

