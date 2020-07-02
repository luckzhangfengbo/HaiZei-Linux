#include "head.h"

extern WINDOW *Write;
extern int sockfd;

void show_strength() {
    int maxx, maxy, strength, mousex = 2, offset = 1;
    getmaxyx(Write, maxy, maxx);
    for (int i = 2; i < maxx - 2; i++) {
        if (i <= maxx / 5 || i >= (maxx / 5) * 4) {
            wattron(Write, COLOR_PAIR(7));
        } else if ((i > maxx / 5 && i < (maxx / 5) * 2) || i > (maxx / 5) * 3 && i < (maxx / 5) * 4) {
            wattron(Write, COLOR_PAIR(10));
        } else {
            wattron(Write, COLOR_PAIR(8));
        }
        mvwprintw(Write, 2, i, " ");
    }
    wattron(Write, COLOR_PAIR(3));
    wrefresh(Write);
    make_nonblock(0);
    while (1) {
        char c = getchar();
        if (c != -1) {
            if (c == ' ' || c == 'k') {
                //
                break;
            }
        }
        usleep(5000);
        if (mousex >= maxx - 2) {
            offset = -1;
        } else if (mousex <= 2) {
            offset = 1;
        }
        mvwprintw(Write, 3, mousex, " ");
        mvwprintw(Write, 1, mousex, " ");
        mousex += offset;
        mvwprintw(Write, 3, mousex, "|");
        mvwprintw(Write, 1, mousex, "|");
        mvwprintw(Write, 4, maxx, " ");
        wrefresh(Write);
    }
    int arr[5] = {1, 2, 3, 2, 1};
    return ;
}
