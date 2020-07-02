#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include "head.h"
#define MAXTASK 100
#define MAXTHREAD 10
/*typedef struct {
    int sum;
    int head;
    int tail;
    int *fd;//存储打开文件，已有连接
    pthread_mutex_t mutex;//互斥量
    pthread_cond_t cond; //互斥条件
}TaskQueue;//任务队列

char ch_char(char c);
void do_echo(int fd);
void TaskQueueInit(TaskQueue *queue, int sum);
void TaskQueuePush(TaskQueue *queue, int fd);
int TashQueuePop(TaskQueue *queue);
void *blue_thread_run(void *arg);
void *red_thread_run(void *arg);*/

struct task_queue{
    int sum;
    int epollfd;
    struct User **team;
    int head;
    int tail;
    pthread_mutex_t mutex;//互斥量
    pthread_cond_t cond; //互斥条件
};

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd);
void task_queue_push(struct task_queue *taskQueue, struct User *user); //直接传结构体，会拷贝一份传过去，比较慢
struct User *task_queue_pop(struct task_queue *taskQueue);
void *thread_run(void* arg);

#endif
