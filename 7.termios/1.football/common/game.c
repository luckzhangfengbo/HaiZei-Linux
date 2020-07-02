#include "head.h"

//#define MAX 50 //Maximum number of users
extern struct Map court;
extern WINDOW *Football, *Football_t, *Message, *Help, *Score, *Write;
//extern char data_stream[20];
int message_num = 0;



WINDOW *create_newwin(int width, int heigth, int startx, int starty) {
    WINDOW *win;
    win = newwin(heigth, width, starty, startx);
    box(win, 0, 0); //当我们创建了一个窗口之后却无法看见它,所以我们现在要做的就是让窗口显示出来。box()函数可以在已定义的窗口外围画上边框。
    wrefresh(win);
    return win;
}

void destroy_win(WINDOW * win) {
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(win);
    delwin(win);
}

void gotoxy(int x, int y) {
    move(y, x);
}

void gotoxy_putc(int x, int y, int c) {
    move(y, x);
    addch(c);
    move(LINES - 1, 1);
    refresh();
}

void gotoxy_puts(int x, int y, char* s) {
    move(y, x);
    addstr(s);
    move(LINES - 1, 1);
    refresh();
}

void w_gotoxy_putc(WINDOW *win, int x, int y, int c){
    mvwaddch(win, y, x, c);//移动到窗口指定位置打印字符
    move(LINES - 1, 1);
    wrefresh(win);
}

void w_gotoxy_puts(WINDOW *win, int x, int y, char *s) {
    mvwprintw(win, y, x, s);
    move(LINES - 1, 1);
    wrefresh(win);
}

void initfootball() {
    initscr();
    clear();
    if (!has_colors() || start_color() == ERR) {
        endwin();
        fprintf(stderr, "your terminal not surport color！\n");
        exit(1);
    }
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, COLOR_GREEN);
    init_pair(8, COLOR_BLACK, COLOR_RED);
    init_pair(9, COLOR_BLACK, COLOR_WHITE);
    init_pair(10, COLOR_BLACK, COLOR_YELLOW);
    init_pair(11, COLOR_BLACK, COLOR_CYAN);
    init_pair(12, COLOR_BLACK, COLOR_BLUE);
    
    //Football = create_newwin(court.width, court.heigth, court.start.x, court.start.y);
    Football_t = create_newwin(court.width + 4, court.heigth + 2, court.start.x - 2, court.start.y - 1);
    Football = subwin(Football_t, court.heigth, court.width, court.start.y, court.start.x);
    box(Football, 0, 0);
    WINDOW *Message_t = create_newwin(court.width + 4, 7, court.start.x - 2, court.start.y + 1 + court.heigth);
    Message = subwin(Message_t, 5, court.width + 2, court.start.y + court.heigth + 2, court.start.x - 1);
    scrollok(Message, 1);
    Help = create_newwin(20, court.heigth + 2, court.start.x + court.width + 2, court.start.y - 1);
    Score = create_newwin(20, 7, court.start.x + 1 + court.width + 1, court.start.y + 2 + court.heigth - 1);
    //WINDOW *Write_t = create_newwin(court.width + 20, 5, court.start.x, court.start.y + court.heigth + 7);
    Write = create_newwin(court.width + 24, 5, court.start.x - 2, court.start.y + 1 + court.heigth + 7);
    //Write = subwin(Write_t, 3, court.width + 20 - 2, court.start.y + court.heigth + 7 + 1, court.start.x + 1);
}

void *draw(void *arg) {
    initfootball();
    while (1) {
        sleep(10);
    }
}

void show_message(WINDOW *win, struct User *user, char *msg,  int type) {  //某个窗口，这是谁User，内容，系统为1
    time_t time_now = time(NULL);
    struct tm* tm = localtime(&time_now);
    char timestr[20] = {0};
    char username[80] = {0};
    sprintf(timestr, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
    if (type) {
        wattron(win,COLOR_PAIR(4));
        strcpy(username, "Server Info : ");
    } else {
        if (user->team)
            wattron(win ,COLOR_PAIR(6));
        else
            wattron(win, COLOR_PAIR(2));
        sprintf(username, "%s : ", user->name);
    }

    if (message_num <= 4) {
        w_gotoxy_puts(win, 10, message_num, username);
        wattron(win, COLOR_PAIR(3));
        w_gotoxy_puts(win, 10 + strlen(username), message_num, msg);
        wattron(win, COLOR_PAIR(5));
        w_gotoxy_puts(win, 1, message_num, timestr);
        message_num++;
    } else {
        message_num = 4;
        scroll(win);
        w_gotoxy_puts(win, 10, message_num, username);
        wattron(win, COLOR_PAIR(3));
        w_gotoxy_puts(win, 10 + strlen(username), message_num, msg);
        wattron(win, COLOR_PAIR(5));
        w_gotoxy_puts(win, 1, message_num, timestr);
        message_num++;
    }
    wrefresh(win);
}

