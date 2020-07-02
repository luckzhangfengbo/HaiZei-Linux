#ifndef _GAME_H
#define _GAME_H
#include <curses.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#define X 70
#define Y 20

struct Point {
    int x, y;
};

struct Point Bpoint;      // Board Position
struct Point Opoint;      // Ball position
struct Point Bdir, Odir;  // Direction of movement

int cx = (X - 2) / 2 + 1;  // X Center
int cy = (Y - 2) / 2 + 1;  // Y Center

int dir_r[3] = {-1, 0, 1};  // Directions for random movement

int flag = 0;  // Ball is moving
int score = 0;

uint32_t get_random(uint32_t max) {
    uint32_t value;
    srand((unsigned)time(NULL));
    value = rand() % (max + 1);
    return value;
}

void gotoxy(int x, int y) { move(y, x); }

void gotoxy_putc(int x, int y, char c) {
    mvaddch(y, x, c);
    move(LINES - 1, 1);
    refresh();
}

void gotoxy_puts(int x, int y, const char *c) {
    mvaddstr(y, x, c);
    move(LINES - 1, 1);
    refresh();
}

void init_field(int x, int y) {
    mvaddch(0, 0, '0');
    // mvaddch(0, y, 'X');
    gotoxy_putc(x + 2, 0, 'X');
    // mvaddch(x, 0, 'Y');
    gotoxy_putc(0, y + 2, 'Y');
    attron(COLOR_PAIR(1));
    for (int i = 1; i <= x; ++i) {
        gotoxy_putc(i, 1, '-');
        gotoxy_putc(i, y, '-');
    }
    for (int i = 1; i <= y; ++i) {
        gotoxy_putc(1, i, '|');
        gotoxy_putc(x, i, '|');
    }
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(4));

    // Information Fields
    gotoxy_puts(2, y + 3, "Message:");
    gotoxy_puts(x + 3, 2, "Help:");
    attroff(COLOR_PAIR(4));
    return;
}

void init_game() {
    if (!has_colors() || start_color() == ERR) {
        endwin();
        fprintf(stderr, "Your terminal does not support colors!\n");
        exit(1);
    }
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    
    init_field(X, Y);

    // Draw board
    Bpoint.x = cx - 3;
    Bpoint.y = Y - 3;
    attron(COLOR_PAIR(3));
    for (int i = 0; i < 8; ++i) {
        gotoxy_putc(Bpoint.x + i, Bpoint.y, ' ');
        gotoxy_putc(Bpoint.x + i, Bpoint.y + 1, ' ');
    }
    attroff(COLOR_PAIR(3));

    refresh();
}

void draw_game() {
    // Clear old ball
    gotoxy_putc(Opoint.x, Opoint.y, ' ');

    //Move with board
    if (!flag) {
        Opoint.x = Bpoint.x + 4;
    }

    // Calculate ball's position
    if (flag) {
        Opoint.x += Odir.x;
        Opoint.y += Odir.y;
    }
    

    // Boundary detection
    if (Opoint.x >= X - 1) {  // Right
        Opoint.x = X - 1;
        Odir.x = -1;
        Odir.y = dir_r[get_random(2)];
    }
    if (Opoint.x <= 2) {  // Left
        Opoint.x = 2;
        Odir.x = 1;
        Odir.y = dir_r[get_random(2)];
    }
    if (Opoint.y <= 1) {  // Top
        Opoint.y = 2;
        Odir.y = 1;
        Odir.x = dir_r[get_random(2)];
    }
    if (Opoint.y >= Y - 1) {  // Bottom
        char info[1024] = {0};
        sprintf(info, "GAME OVER! Score = %d\n", score);
        gotoxy_puts(3, Y + 4, info);
        flag = 0;
        score = 0;
        Opoint.x = cx;
        Opoint.y = Y - 4;

        for (int i = 0; i < 8; ++i) {
            gotoxy_putc(Bpoint.x + i, Bpoint.y, ' ');
            gotoxy_putc(Bpoint.x + i, Bpoint.y + 1, ' ');
        }
        Bpoint.x = cx - 3;
        Bpoint.y = Y - 3;
    }

    // Draw new ball
    attron(COLOR_PAIR(4));
    gotoxy_putc(Opoint.x, Opoint.y, 'o');
    attroff(COLOR_PAIR(4));

    // Clear old board
    attron(COLOR_PAIR(2));
    for (int i = 0; i < 8; ++i) {
        gotoxy_putc(Bpoint.x + i, Bpoint.y, ' ');
        gotoxy_putc(Bpoint.x + i, Bpoint.y + 1, ' ');
    }
    attroff(COLOR_PAIR(2));

    // Caculate board's position
    Bpoint.x += Bdir.x;
    Bpoint.y += Bdir.y;

    // Clear direction & Boundary detection
    memset(&Bdir, 0, sizeof(Bdir));
    if (Bpoint.x >= X - 8) Bpoint.x = X - 8;
    if (Bpoint.x <= 2) Bpoint.x = 2;

    // Draw new board
    attron(COLOR_PAIR(3));
    for (int i = 0; i < 8; ++i) {
        gotoxy_putc(Bpoint.x + i, Bpoint.y, ' ');
        gotoxy_putc(Bpoint.x + i, Bpoint.y + 1, ' ');
    }
    attroff(COLOR_PAIR(3));

    // Catch ball
    if (flag && Opoint.x <= Bpoint.x + 8 && Opoint.x >= Bpoint.x &&
        Opoint.y >= Y - 4) {
        score += 100;
        Odir.y = -1;
        Odir.x = dir_r[get_random(2)];
        char info[1024] = {0};
        sprintf(info, "You get %d, Total %d!", 100, score);
        gotoxy_puts(3, Y + 4, info);
    }
}

#endif
