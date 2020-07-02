#include "game.h"

int main() {
    initscr();
    noecho();
    cbreak();//禁用行缓冲
    keypad(stdscr, TRUE);//启用功能键
    init_game();
    signal(SIGALRM, draw_game); //draw_game()函数用来重绘画面

    struct itimerval itimer;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 100000;
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 1000;

    setitimer(ITIMER_REAL, &itimer, NULL);
    
    //Initial position of the ball
    Opoint.x = cx;
    Opoint.y = Y - 4;

    //Draw a ball
    attron(COLOR_PAIR(4));
    gotoxy_putc(Opoint.x, Opoint.y, 'o');
    attroff(COLOR_PAIR(4));

    refresh();

    while (1) {
        int c = getch();
        switch (c) {
            case KEY_LEFT: {
                Bdir.x -= 2;
            } break;
            case KEY_RIGHT: {
                Bdir.x += 2;
            } break;
            case KEY_UP: {
                if (flag) break;
                flag = 1;
                Odir.x = dir_r[get_random(2)];
                Odir.y = -1;
                
                //char blank[] =  "                           ";
                //Odir.x = dir_r[get_random(12)];
                //Odir.y =

            } break;
            case 'q': {
                endwin();
                exit(0);
            } break;
            default: {
            } break;
        }
    }
    getch();
    endwin();
    return 0;
}
