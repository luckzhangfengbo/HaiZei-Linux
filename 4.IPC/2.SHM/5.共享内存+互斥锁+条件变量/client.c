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
    if((shmid = shmget(key, sizeof(struct Shm),IPC_CREAT|0666)) < 0) {
        perror("shmget");
        return 1;
    }
    if((share_memory = shmat(shmid, NULL, 0)) == (struct Shm *)-1) {
        perror("shmat");
        return 1;
    }

    while(1) {
        printf("Before mutex lock!\n");
         pthread_mutex_lock(&share_memory->mutex);
         printf("<client> : ");
         scanf("%[^\n]s",share_memory->buff);
         getchar();
         pthread_cond_signal(&share_memory->cond);
         //sleep(1);
        pthread_mutex_unlock(&share_memory->mutex);

    }
    return 0;
}

