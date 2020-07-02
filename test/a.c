#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/wait.h>

int main(int argc, char **argv){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s sum number", argv[0]);
        return 1;
    }
    pid_t pid;
    FILE *fp = NULL;
    int sum = 0;
    char buf[5] = {0};
    for (int i = 0; i < *argv[2]; i++) {
        if ((pid = fork() < 0)) {
            perror("fork");
        }
        if (pid == 0) {
            if ((fp = fopen("./sum", "rw")) == NULL) {
                perror("fopen");
                exit(1);
            }
            flock(fileno(fp), LOCK_EX);
            read(fileno(fp), (void *)buf, sizeof(buf));
            sum = atoi(buf);
            printf("%d\n", sum);
            sum += 2;
            sum += '0';
            write(fileno(fp),(void *)buf, sizeof(buf));
            flock(fileno(fp), LOCK_UN);
            fclose(fp);
            exit(0);
        }
        wait(NULL);
    }

    return 0;
}
