#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define CNT 256

char filename[CNT];
char name[CNT];
char tmp[6];
char cc_flag[5];
char xname[CNT] = {'.', '/'};

int main(int argc, char **argv) {
    pid_t pid;
    if (argc != 2) {
        printf("Usage: ./a.out condefile\n");
        exit(1);
    }
    strcpy(filename, argv[1]);

    char *buff;
    buff = strstr(filename, ".");
    strncpy(name, filename, buff - filename);
    strcpy(tmp, buff);

    if (!strcmp(tmp, ".cpp")) {
        strcpy(cc_flag, "g++");
    }else if (!strcmp(tmp, ".c")) {
        strcpy(cc_flag, "gcc");
    }else {
        printf("Not Support File Type\n");
        exit(2);
    }
    
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    }
    if (pid == 0) {
        execlp("vim", "vim", filename, NULL);
    } else {
        if (waitpid(pid, NULL, 0) == pid) {
            //printf("Father Process!\n");
            if((pid = fork()) < 0) {
                perror("fork error");
                exit(1);
            }
            if (pid == 0) {
                //printf("gcc/g++!\n");
                if (!strcmp(cc_flag, ".c")) {    
                    execlp("gcc", "gcc", filename, "-o", name, NULL);
                } else {
                    execlp("g++", "g++", filename, "-o", name, NULL);
                }
            } else {
                if (waitpid(pid, NULL, 0) == pid) {
                    if ((pid = fork()) < 0) {
                        perror("fork error");
                        exit(1);
                    }
                    if (pid == 0) {
                        strcat(xname, name);
                        /*for (int i = 0; i < strlen(xname); i++) {
                            printf("%c", xname[i]);
                        }
                        printf("\n");*/
                        execlp(xname, name, NULL);
                    }
                    wait(NULL);
                }
            }
            //wait(NULL);
        }
    }
    //wait(NULL);

    return 0;
}
