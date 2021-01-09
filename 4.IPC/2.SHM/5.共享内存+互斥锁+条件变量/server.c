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
#include<pthread.h>
struct Shm{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char buff[1024];
};

struct Shm *share_memory = NULL;

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
    memset(share_memory, 0, sizeof(struct Shm));
    pthread_mutexattr_t m_attr;
    pthread_condattr_t c_attr;

    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);
    
    pthread_mutexattr_setpshared(&m_attr,PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&share_memory->mutex, &m_attr);
    pthread_cond_init(&share_memory->cond, &c_attr);

    while(1){
        pthread_mutex_lock(&share_memory->mutex);
        printf("After muter lock!\n");
        pthread_cond_wait(&share_memory->cond, &share_memory->mutex);
        printf("After cond wait");
        //sleep(2);
        printf("<Recv> : %s .\n", share_memory->buff);
        pthread_mutex_unlock(&share_memory->mutex);
    }
    return 0;
}

