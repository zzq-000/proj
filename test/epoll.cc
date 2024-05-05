#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>

// #include "Packet.h"
using namespace std;
int main() {
    int epfd = epoll_create1(0);

    int sock_fd;

    struct sockaddr_in servaddr, cliaddr;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // 填充服务器信息
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8000);

    
    if (bind(sock_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN; // 监听读事件
    ev.data.fd = sock_fd; // 监听的文件描述符

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev) == -1) {
        perror("epoll_ctl: listen_fd");
        exit(EXIT_FAILURE);
    }

    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];
    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == sock_fd) {
                cout << "hh" << endl;
                uint8_t buffer[BUFSIZ];
                int size = read(sock_fd, buffer, BUFSIZ);
            }
        }
    }

}