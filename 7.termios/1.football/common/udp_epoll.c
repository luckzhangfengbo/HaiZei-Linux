






#include "./color.h"
#include "./common.h"
#include "./udp_client.h"
#include "./udp_server.h"
#include "./head.h"
#define MAX 50
extern int port; //port是在其他.c文件中定义的
extern struct User *rteam;
extern struct User *bteam;
extern int repollfd, bepollfd;

int socket_create_udp(int port);

void add_event(int epollfd, int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void add_event_ptr(int epollfd, int fd, int events, struct User *user) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = (void *)user; //注册指针，指向用户的存储空间
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    DBG(GREEN"Sub Thread"NONE" : After EPoll Add %s.\n", user->name);
}

void del_event(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

/*void del_event_ptr(int epollfd, int fd, int events, struct User *user) {
    struct epoll_event ev;
    ev.data.ptr = (void *)user;
    ev.events = events;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
    DBG(GREEN"Sub Thread"NONE" : After EPoll Del %s.\n", user->name);
}*/

int udp_connect(int epollfd, struct sockaddr_in * serveraddr) {
    int sockfd;
    if((sockfd = socket_create_udp(port)) < 0) { //创建一个bind状态的udp
        perror("socket_udp");
        return -1;
    }
    if (connect(sockfd, (struct sockaddr *)serveraddr, sizeof(struct sockaddr)) < 0) {
        perror("connect");
        return -1;
    }
    DBG(GREEN"INFO"NONE" : After connect.\n");
    //send(sockfd, "Login!", sizeof("Login!"), 0);
    //add_event(epollfd, sockfd, EPOLLIN);
    return sockfd;
}

int check_online(struct LogRequest *request) {
    for (int i = 0; i < MAX; i++) {
        if (rteam[i].online && !strcmp(rteam[i].name, request->name)) return 1;
        if (bteam[i].online && !strcmp(bteam[i].name, request->name)) return 1;
    }
    return 0; //未在线
}

int udp_accept(int epollfd, int fd, struct User *user) {
    struct sockaddr_in client;
    int new_fd, ret;
    struct LogRequest request;
    struct LogResponse response;

    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));

    socklen_t len = sizeof(struct sockaddr_in);
    ret = recvfrom(fd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&client, &len);
    if (ret != sizeof(request)) {
        response.type = 1;
        strcpy(response.msg, "Login failed.");
        sendto(fd,(void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }

    if (check_online(&request)) {
        response.type = 1;
        strcpy(response.msg, "You Alreadly Playing This Game SomeWhere!");
        sendto(fd,(void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }

    response.type = 0;
    strcpy(response.msg, "Login success. Enjoy youself.");
    sendto(fd,(void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
    
    if (request.team)
        DBG(GREEN"INFO"NONE" : "BLUE" %s on %s:%d longin! (%s)\n"NONE, request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), response.msg);
    else
        DBG(GREEN"INFO"NONE" : "RED" %s on %s:%d longin! (%s)\n"NONE, request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), response.msg);
    
    strcpy(user->name, request.name);
    user->team = request.team;
    new_fd = udp_connect(epollfd, &client);
    user->fd = new_fd;
    return new_fd;
}

int find_sub(struct User *team) {
    for (int i = 0; i < MAX; i++) {
        if (!team[i].online) return i;
    }
    return -1;
}

void add_to_sub_reactor(struct User *user) {
    struct User *team  = (user->team ? bteam : rteam);
    DBG(YELLOW"Main Thread : "NONE"Add to sub_reactor\n");
    int sub = find_sub(team);
    team[sub] = *user; //把user结构拷贝一份存到数组中，数组中存的就是结构体
    team[sub].online = 1;
    team[sub].flag = 10;
    DBG(L_RED"sub = %d, name = %s\n"NONE, sub, team[sub].name);
    if (user->team)
        add_event_ptr(bepollfd, team[sub].fd, EPOLLIN | EPOLLET, &team[sub]);
    else
        add_event_ptr(repollfd, team[sub].fd, EPOLLIN | EPOLLET, &team[sub]);
}

