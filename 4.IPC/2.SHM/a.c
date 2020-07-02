






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
    printf("<Recv> : %s\n", share_memory->buff);
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

    pthread_mutexattr_t m_attr;
    pthread_condattr_t c_attr;

    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);

    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&share_memory->mutex, &m_attr);
    pthread_cond_init(&share_memory->cond, &c_attr);
    
    while (1) {
    pthread_mutex_lock(&share_memory->mutex);
    //printf("After mutex lock\n");
    pthread_cond_wait(&share_memory->cond, &share_memory->mutex);
    //printf("After cond wait\n");
    //sleep(2);
    print();
    pthread_mutex_unlock(&share_memory->mutex);
    }
    return 0;
}
