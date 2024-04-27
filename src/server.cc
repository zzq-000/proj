#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Packet.h"
using namespace std;
int test1_endian() {
    unsigned int x = 1; // 使用一个整数，并将其值设为1
    char *c = (char*)&x; // 获取该整数的地址，并将其视为字符型指针
    if (*c) {
        printf("Little endian\n");
    } else {
        printf("Big endian\n");
    }
    return 0;
}
int main() {
    test1_endian();
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    // 创建 socket 文件描述符
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // 填充服务器信息
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    // 绑定 socket 与地址
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    while (true) {
        unsigned int len = sizeof(cliaddr);
        Packet p;
        int n = recvfrom(sockfd, (void *)&p, sizeof(p), MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
        // cout << "n: " << n << endl;
        // r = ntohs(r);
        // cout << " r: " << r << endl;
        cout << n << endl;
        cout << sizeof(Packet) << endl;
        if (n == sizeof(Packet)) {
            // cout << "hello" << endl;
            // cout << "hello" << endl;
            // Packet* p = (Packet*)buffer;
            p.len = ntohl(p.len);
            cout << p.len << endl;;
            // cout << "hello" << endl;
            // for (int i = 0; i < 5; ++i) {
            //     cout << "h";
            // }
            for (uint32_t i = 0; i < p.len; i++) {
                cout << static_cast<uint16_t>(p.data[i]) << " ";
                cout.flush();
            }
            // cout << "h";
            // for (int i = 0;)
        }
        // std::cout << " Client : " << ntohs(buffer[0]) << " " << ntohs(buffer[1]) << std::endl;
    }

    close(sockfd);
    return 0;
}
