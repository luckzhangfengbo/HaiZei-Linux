#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct Msg{
    int age;
    char name[20];
};

int retval = 3;

void *print(void *arg) {
    struct Msg *tmp;
    tmp = (struct Msg *)arg;
    printf("name = %s, age = %d\n", tmp->name, tmp->age);
    //int retval = 3;
    pthread_exit((void *)&retval);
    //return NULL;
}

int main() {
    void *status;
    pthread_t tid;
    struct Msg msg;
    msg.age = 18;
    strcpy(msg.name, "zhangzhiguo");
    pthread_create(&tid, NULL, print, (void *)&msg);
    //sleep(1);
    pthread_join(tid, &status); //wait
    printf("%d\n", *(int *)status);
    return 0;
}
