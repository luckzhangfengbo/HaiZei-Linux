#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    int pid, x = 0;
    for (int i = 1; i <= 10; i++) {
        if ((pid = fork()) < 0) {
            perror("fork");
            continue;
        }
        if (pid == 0) {
            x = i;
            break;
        }
    }
    printf("I'm %dth Child.\n", x);
    sleep(60);
    return 0;
}
