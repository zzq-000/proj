#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <iostream>


#include "udp_socket.hh"
#include "Packet.h"
#include "protocol.h"
#include "glog/logging.h"
#include "receiver.h"
using namespace std;
int main() {
    Address addr("127.0.0.1", 8000);
    UDPSocket udp;

    Packet p = Protocol::RegisterSessionRequestPacket(100);
    
    udp.sendto(addr, &p, p.PacketSize());
    // LOG(INFO) << p.PacketSize() << " " << p.IsStart();
    auto info = udp.recv();
    Packet resp = Packet::ParseFromData(info.value().data(), info.value().size());

    if (resp.GetPacketType() == Packet_type::SessionResponsePacket) {
        cout << "session response" << endl;
    }
    Receiver receiver("non.txt", Fec_type::FEC_2_1, udp, addr);
    receiver.KeepReceive();
    // 接收服务器回复
    // char buffer[1024];
    // socklen_t len = sizeof(servaddr);
    // int n = recvfrom(sockfd, (char *)buffer, 1024, 
    //                  MSG_WAITALL, (struct sockaddr *) &servaddr,
    //                  &len);
    // buffer[n] = '\0';
    // std::cout << "Server : " << buffer << std::endl;

    return 0;
}
