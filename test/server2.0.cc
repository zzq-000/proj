#include <iostream>
#include <sys/epoll.h>
#include "udp_socket.hh"
#include "Packet.h"
#include "protocol.h"
#include "glog/logging.h"
#include "sender.h"
#include "timestamp.hh"
using namespace std;

int main() {
    Address server_addr("127.0.0.1", 8000);
    UDPSocket server;
    server.bind(server_addr);
    
    int epfd = epoll_create1(0);

    struct epoll_event ev;
    ev.events = EPOLLIN; // 监听读事件
    ev.data.fd = server.fd_num(); // 监听的文件描述符
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server.fd_num(), &ev) == -1) {
        perror("epoll_ctl: listen_fd");
        exit(EXIT_FAILURE);
    }

    int MAX_EVENTS = 100;
    struct epoll_event events[MAX_EVENTS];
    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server.fd_num()) {

                uint8_t buffer[BUFSIZ];
                auto [address, info] = server.recvfrom();
                Sender sender("non.txt", FecType::FEC_2_1, address, 5 * 1024 * 1024);
                Packet p = Packet::ParseFromData(info.value().data(), info.value().size());
                if (p.GetPacketType() == PacketType::SessionRequestPacket) {
                    LOG(INFO) << "received a SessionRequestPacket from: " << address.str();
                    Packet ack;
                    auto rtn = Protocol::RegisterSessionResponsePacket(p, ack);

                    if (rtn == Return_Code::SUCCESS) {
                        server.sendto(address, &ack, ack.PacketSize());
                        LOG(INFO) << "send a SessionResponsePacket to: " << address.str();

                        SleepMs(1000);

                        sender.KeepSend(ack.GetSequenceNum() + 1);

                    }
                }
            }
        }
    }


}