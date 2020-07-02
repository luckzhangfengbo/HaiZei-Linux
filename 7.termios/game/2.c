
#include<stdio.h>
#include<unistd.h>
#include<ncurses.h>
#define LEFIEDGE 10
#define RIGHTEDGE 70
int main() {
    char message[20] = "**";
    char blank[20] = " ";
    int col = 0;
    int row = 0;
    int dirx = 1;
    int diry = 1;
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_RED);
    while(1) {
        move(row, col);
        attron(COLOR_PAIR(1));
        addstr(message);
        attroff(COLOR_PAIR(1));
        move(LINES - 1, COLS - 1);
        refresh();
        usleep(10000);
        move(row, col);
        addstr(blank);
        col += dirx;
        row += diry;
        if(col >= RIGHTEDGE) {
            dirx = -1;
        }
        if(col <= LEFIEDGE)dirx = +1;
        if(row > LINES - 1) diry = - 1;
        if(row <= 1) diry = +1;
    }

    return 0;
}

