#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

constexpr int dataLen = 1000;
struct Packet {
    int len;
    uint8_t data[dataLen];
};
int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // 创建 socket 文件描述符
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // 服务器信息
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 发送消息
    // const char *message = "Hello, Server!";
    while(true) {
        Packet p;
        p.len = 10;
        for (int i = 0; i < p.len; i++) {
            p.data[i] = i;
        }
        p.len = htonl(p.len);
        // uint16_t x = 10;
        // std::cin >> x ;
        // x = htons(x);
        sendto(sockfd, (void*)&p, sizeof(p), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        std::cout << "Message sent to server" << std::endl;
        break;
    }

    close(sockfd);
    return 0;
}
