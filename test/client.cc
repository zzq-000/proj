#include <iostream>
#include <cstring>
#include <unistd.h>


#include "glog/logging.h"
#include "udp_socket.hh"
#include "sender.h"

using namespace std;
// constexpr int dataLen = 1000;
// struct Packet {
//     uint16_t len;
//     uint8_t data[dataLen];
// };

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
     FLAGS_log_dir = "./"; 
    Address address("127.0.0.1", 8000);
    string filename;
    cin >> filename;
    Sender s(filename, Fec_type::FEC_2_1, address);
    // UDPSocket client_socket;
    s.SendTo(address);
    return 0;
}
