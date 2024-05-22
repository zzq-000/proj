#include "packet.pb.h"
#include <iostream>
#include <fstream>
#include <string>


using namespace std;
int main() {
    Packet p;
    p.set_fec_index(1);
    p.set_seq_num(2);
    p.set_packet_type(PacketType::DataPacketId);
    DataPacket* data = new DataPacket;

    string s("hello, world ");

    data->set_data(s);
    data->set_len(s.size());
    p.set_allocated_data_packet(data);
    // p.set_data_packet(data);

    cout << p.ByteSizeLong() << endl;
}