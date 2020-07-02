






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

void print(int sig) {
    printf("<server> : %s\n", share_memory->buff);
    memset(share_memory->buff, 0, sizeof(share_memory->buff));
}

int main() {
    int shmid;
    key_t key = ftok(".", 198);

    if ((shmid = shmget(key, sizeof(struct Shm), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }
    printf("After shmget!\n");
    if ((share_memory = (struct Shm *)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat");
        return 1;
    }
    printf("After shmat!\n");
    //memset(share_memory, 0, sizeof(struct Shm));
    share_memory->pid = getpid();
    while (1) {
    signal(SIGUSR2, print);
    pause();
    }
    return 0;
}
