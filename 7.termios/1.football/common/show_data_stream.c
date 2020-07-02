#include "head.h"

extern char data_stream[20];
extern WINDOW *HELP;
extern struct Map court;

/* 'l'ogin, 'c'arry, 'k'ick, 's'top, 'n'ormal, 'e'xit */

void show_data_stream(int type) {
    for (int i = 19; i > 0; --i) {
        data_stream[i] = data_stream[i - 1];
    }
    data_stream[0] = type;
    for (int i = 1; i < 19; i++) {
        switch(data_stream[i]) {
            case 'l':{
                wattron(Help, COLOR_PAIR(7));
            }break;
            case 'c':{
                wattron(Help, COLOR_PAIR(8));
            }break;
            case 'k':{
                wattron(Help, COLOR_PAIR(9));
            }break;
            case 's':{
                wattron(Help, COLOR_PAIR(10));
            }break;
            case 'n':{
                wattron(Help, COLOR_PAIR(11));
            }break;
            case 'e':{
                wattron(Help, COLOR_PAIR(12));
            }break;
        }
        w_gotoxy_putc(Help, i, court.heigth, ' ');
        wrefresh(Help);
    }
    return ;
}
