#include <iostream>
#include <cstring>
#include <unistd.h>


#include "glog/logging.h"
#include "udp_socket.hh"
#include "epoller.hh"
#include "sender.h"

using namespace std;
// constexpr int dataLen = 1000;
// struct Packet {
//     uint16_t len;
//     uint8_t data[dataLen];
// };



int main(int argc, char** argv) {
    // cout << argv[0] << endl;
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = "./"; 
    Address address("127.0.0.1", 8000);
    UDPSocket sock;
    sock.bind(address);

    Epoller ep;

    ep.register_event(sock, Epoller::In, [&sock](){
        auto [addr, info] = sock.recvfrom();
        LOG(INFO) << addr.str();
        
    });
    ep.poll(-1);





    // string filename = "hello";
    // // cin >> filename;
    // // Sender s;
    // Sender s(filename, Fec_type::FEC_2_1, address, 5 * 1024 * 1024);
    // // // UDPSocket client_socket;
    // s.KeepSend();
    return 0;
}
