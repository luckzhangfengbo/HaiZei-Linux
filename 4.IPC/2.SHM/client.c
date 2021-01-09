
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

struct Shm {
    pid_t pid;
    char buff[1024];
};

struct Shm *share_memory = NULL;

int main() {
    int shmid;
    key_t key = ftok(".", 198);

    if ((shmid = shmget(key, sizeof(struct Shm), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }
    if ((share_memory = (struct Shm *)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat");
        return 1;
    }
    //shmget 申请的空间并没有清空
    //memset(share_memory, 0, sizeof(struct Shm));
    

    while (1) {
        printf("<client> : ");
        scanf("%[^\n]s", share_memory->buff);
        getchar();
        if (share_memory->pid == 0) {
            printf("<Error> : 服务端没上线!\n");
        } else {
            kill(share_memory->pid, SIGUSR2);
        }
    }
    return 0;
}
