#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXLINE 1024

FILE *mypopen(char *cmd,char type) {
    printf("start!\n");
	int pipefd[2];
	pid_t pid;

	if(type !='r' && type != 'w') {
		printf("myopen() type error/n");
		return NULL;
	}

	if(pipe(pipefd)<0) {
		perror("pipe");
        return NULL;
	}
	
    if ((pid = fork()) < 0) {
        perror("fork");
        return NULL;
    }

	if(0 == pid) {
	    if(type == 'r') { //返回的文件指针用于读,所以应该往里面写
			close(pipefd[0]);           
			dup2(pipefd[1], STDOUT_FILENO);  
			close(pipefd[1]);		

		}
		else {
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
		}
        printf("Befor execv!\n");
		if(execlp(cmd, cmd, NULL) < 0)          //用exec族函数执行命令
		    return NULL;	
	}
	printf("After execv!\n");
	wait(0);                                //等待子进程返回

	if(type=='r'){
		close(pipefd[1]);
	    printf("Befor return!\n");
		return fdopen(pipefd[0],"r");	//返回文件指针
    }else{
		close(pipefd[0]);
		return fdopen(pipefd[1],"w");
	}
}

void err_quit(const char *str) {
    perror(str);
    exit(1);
}

int main() {
    char line[MAXLINE];
    FILE *fp = NULL;

    if ((fp = mypopen("ls", 'r')) == NULL) {
        perror("mypopen");
        exit(1);
    }
    //fp = mypopen("ls -l", 'r');
    while (fgets(line, MAXLINE, fp) != NULL) {
        fputs(line, stdout);
    }
    return 0;
}
