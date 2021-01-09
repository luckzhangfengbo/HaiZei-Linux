/*************************************************************************
	> File Name: a.c
	> Author: zhangfb
	> Mail: 1819067326
	> Created Time: 六  1/ 9 10:21:49 2021
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>


int main()  {
    FILE *f = fopen("./a.txt", "r");
    flock(f->_file, LOCK_EX);
    printf("After, flock()!\n");
    sleep(10);
    return 0;
}
