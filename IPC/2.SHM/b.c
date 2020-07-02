






#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

struct Shm {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char buff[1024];
};

struct Shm *share_memory = NULL;

void print() {
    printf("<server> : %s\n", share_memory->buff);
    memset(share_memory->buff, 0, sizeof(share_memory->buff));
}

int main() {
    int shmid;
    key_t key = ftok(".", 199);


    if ((shmid = shmget(key, sizeof(struct Shm), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }
    //printf("After shmget!\n");
    if ((share_memory = (struct Shm *)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat");
        return 1;
    }
    //printf("After shmat!\n");

    memset(share_memory, 0, sizeof(struct Shm));

    while (1) {
        //printf("Before mutex lock!\n");
        usleep(10000);
        pthread_mutex_lock(&share_memory->mutex);
        //printf("After mutex lock!\n");
        printf("<client> : ");
        scanf("%[^\n]s", share_memory->buff);
        getchar();
        pthread_cond_signal(&share_memory->cond);
        pthread_mutex_unlock(&share_memory->mutex);
        //pthread_cond_signal(&share_memory->cond);
    }
    return 0;
}
