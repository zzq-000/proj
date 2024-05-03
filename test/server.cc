#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "udp_socket.hh"
#include "receiver.h"
#include "glog/logging.h"
using namespace std;

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = "./"; 
    UDPSocket server_socket;
    Address address("127.0.0.1", 8000);
    Receiver receiver("2.txt", Fec_type::FEC_2_1, address);
    receiver.KeepReceive();
    return 0;
}
