#ifndef _DATATYPE_H
#define _DATATYPE_H
#define MAXMSG 1024

struct LogRequest{
    char name[20]; //Nmae of user
    int team; //0 READ 1 BLUE
    char msg[512];
};

struct LogResponse{
    int type; //0 success 1 failed
    char msg[512];
};

struct Point{
    int x;
    int y;
};

struct User{
    int team;
    int fd;
    char name[20];
    int online;
    int flag; //未效应次数,判断是否在线
    //struct sockaddr_in addr;
    struct Point loc;
};

struct Map {
    int width; //Column
    int heigth; //Row
    struct Point start; //已知起点，长，宽，便可以绘制一个矩形
    int gate_width;
    int gate_heigth;
};

#define ACTION_KICK 0x01
#define ACTION_CARRY 0x02
#define ACTION_STOP 0x04

//Operate the ball
struct Ctrl {
    int action;
    int dirx;
    int diry;
    int strength;
};

/*Mask*/
#define FT_TEST 0x01 //服务端心跳信息
#define FT_WALL 0x02 //服务端以字符串方式广播游戏情况
#define FT_MSG 0x04 //客户端发送的信息，服务端转发的信息
#define FT_ACK 0x08 //客户端对心跳ACK确认
#define FT_FIN 0x10 //客户端，服务端下线前，对彼此的告知
#define FT_CTL 0x20 //客户端发送的控制信号json发送
#define FT_GAME 0x40 //服务端向客户端广播游戏实时图画 json发送
#define FT_SCORE 0x80 //服务端对客户端广播游戏比分变化，json发送（包括进球人）
#define FT_GAMEOVER 0x100 //游戏结束

struct FootBallMsg {
    int type;
    int size;
    int team;
    char name[20];
    char msg[MAXMSG];
    struct Ctrl ctl;
};

#endif
