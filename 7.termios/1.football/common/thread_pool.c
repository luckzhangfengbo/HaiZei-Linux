#include "../common/head.h"
#include "udp_epoll.h"
#include "game.h"

#define MAX 50
extern int repollfd, bepollfd;
extern struct User *rteam, *bteam;

int ch = 'n';

//#define MAXTASK 100
//#define MAXTHREAD 20

void do_echo(struct User *user) {
    struct FootBallMsg msg;
    char tmp[512] = {0};
    //memset(msg.msg, 0, sizeof(msg.msg));

    int size = recv(user->fd, (void *)&msg, sizeof(msg), 0);
    user->flag = 10; //防止下线后就再也上不来
    if (msg.type & FT_ACK) {
        ch = 'l';
        show_data_stream(ch);
        if (user->team)
            DBG(L_BLUE" %s "NONE"❤️\n",user->name);
        else
            DBG(L_RED" %s "NONE"❤️\n",user->name);
    } else if (msg.type & (FT_WALL | FT_MSG)) {
        if (user->team)
            DBG(L_BLUE" %s : %s \n"NONE, user->name, msg.msg);
        else
            DBG(L_RED" %s : %s \n"NONE, user->name, msg.msg);     
        
        strcpy(msg.name, user->name);
        msg.team = user->team;
        //sendall(user);
        Show_Message(, user, msg.msg, );
        
        for (int i = 0; i < MAX; i++) {
            if (rteam[i].online == 1) send(rteam[i].fd, (void *)&msg, sizeof(msg), 0);
            if (bteam[i].online == 1) send(bteam[i].fd, (void *)&msg, sizeof(msg), 0);
        }
        //send(user->fd, (void *)&msg, sizeof(msg), 0);
        
    } else if (msg.type & FT_FIN) {
        ch = 'e';
        show_data_stream(ch);
        DBG(RED"%s logout.\n", user->name);
        sprintf(tmp, "%s Logout.", user->name);
        Show_Message(, NULL, tmp, 1);
        user->online = 0;
        int epollfd_tmp = (user->team ? bepollfd : repollfd);
        del_event(epollfd_tmp, user->fd);
    } else if (msg.type & FT_CTL) {
        show_data_stream(ch);
        Show_Message(, user, "Ctrl Message", 0);
        if (msg.ctl.dirx || msg.ctl.diry) { //这样写线程安全吗？
            user->loc.x += msg.ctl.dirx;
            user->loc.y += msg.ctl.diry;
            if (user->loc.x <= 1) user->loc.x = 1;
            if (user->loc.x >= court.width - 1) user->loc.x = court.width - 1;
            if (user->loc.y <= 1) user->loc.y = 1;
            if (user->loc.y >= court.heigth - 1) user->loc.y = court.heigth - 1;
        }
    }
}

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd) {
    taskQueue->sum = sum;
    taskQueue->epollfd = epollfd;
    taskQueue->team = calloc(sum, sizeof(void *)); //存放指针
    taskQueue->head = taskQueue->tail = 0;
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
}

void task_queue_push(struct task_queue *taskQueue, struct User *user) {
    pthread_mutex_lock(&taskQueue->mutex);
    taskQueue->team[taskQueue->tail] = user;
    DBG(L_GREEN"Thread Pool :"NONE" Task Push %s\n", user->name);
    if (++taskQueue->tail == taskQueue->sum) { //任务队列到头了
        DBG(L_GREEN"Thread Pool :"NONE "Task Queue End.\n");
        taskQueue->tail = 0;
    }
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

struct User *task_queue_pop(struct task_queue *taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
    while (taskQueue->tail == taskQueue->head) {
        DBG(L_GREEN"Thread Pool :"NONE" Task Queue Empty, Waiting For Task.\n");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }
    struct User *user = taskQueue->team[taskQueue->head];
    DBG(L_GREEN"Thread Pool :"NONE" Task Pop %s.\n", user->name);
    if (++taskQueue->head == taskQueue->sum) {
        DBG(L_GREEN"Threed Pool :"NONE" Task Queue End.\n");
        taskQueue->head = 0;
    }
    pthread_mutex_unlock(&taskQueue->mutex);
    return user;
}

void *thread_run(void *arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid);
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while (1) {
        struct User *user = task_queue_pop(taskQueue);
        do_echo(user);
    }
}
