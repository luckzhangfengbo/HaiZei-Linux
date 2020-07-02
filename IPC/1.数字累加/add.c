






#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/file.h>

char num_file[] = "./.num";//隐藏文件
char lock_file[] = "./.lock";

struct Num{
    int now;
    int sum;
};

size_t get_num(struct Num *num) {
    size_t read;
    FILE *f = fopen(num_file, "r");
    if (f == NULL) {
        perror("num_file");
        return -1;
    }
    if ((read = fread(num, sizeof(Num), 1, f)) < 0) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return read;
}

size_t set_num(struct Num *num) {
    FILE *f = fopen(num_file, "wb");//b也可以不用
    size_t write_size = fwrite(num, sizeof(struct Num), 1, f);
    fclose(f);
    return write_size;
}

void do_add(int max, int x){
    struct Num num;
    while (1) {
        FILE *lock = fopen(lock_file, "w"); //锁开始的地方
        flock(lock->_fileno, LOCK_EX);//FILE * 结构体指针，具体内容百度
        if (get_num(&num) < 0) {
            fclose(lock); //关闭文件的时候，锁也会随之消失
            continue;
        }
        printf("<%d> : %d %d \n", x, num.now, num.sum);
        if (num.now > max){
            break;
        }
        num.sum += num.now;
        num.now++;
        printf("write: %d\n", set_num(&num));
        //set_num(&num);
        flock(lock->_fileno, LOCK_UN);
        fclose(lock);
    }
}

int main(int argc, char **argv){
    if (argc != 3) {
        fprintf(stderr, "Usage:%s max ins\n", argv[0]);
        return 1;
    }
    int x = 0;
    int max = atoi(argv[1]);
    int ins = atoi(argv[2]);
    pid_t pid;
    struct Num num;
    num.now = 0;
    num.sum = 0;
    //printf("write: %d\n", set_num(&num));
    set_num(&num);//先把结构体写进文件
    for (int i = 0; i < ins; i++) { //父进程循环创建
        if ((pid = fork()) < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            x = i;
            break;//每个子进程不再去循环创建子进程
        }
    }
    
    if (pid == 0) {
        do_add(max, x);
        printf("<%d> exit!\n", x);
        exit(0);
    }
    while(ins) { //每个子进程都需要等
        wait(NULL);
        ins--;
    }
    //printf("read: %d\n", get_num(&num));
    get_num(&num);
    printf("Ans = %d\n", num.sum);
    //
    //
    return 0;//主函数退出时，会关闭所有打开的文件
}
