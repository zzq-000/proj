#include "Datagram.h"
#include "FecCodec.h"
#include "FecDatagram.h"
#include <vector>
#include <fstream>
#include <gtest/gtest.h>
using namespace std;

TEST(Codec, basic) {
    std::ifstream file("as.bin");
    vector<Datagram*> ans;
    char ch;
    while(!file.eof()) {
        Datagram* a = new Datagram;
        file.read(reinterpret_cast<char*>(a), sizeof(Datagram));
        file.read(&ch, 1);
        if (ch != '\n') {
            cout << "error";
        }
        // a.LoadFromFile("as.obj");
        ans.push_back(a);
    } 
    EXPECT_EQ(1, true);
    LOG(ERROR) << ans.size() << endl;
    // while(true) {
    // }
    ans.pop_back();
    vector<FecDatagram*> data(ans.size());
    for (int i = 0; i < ans.size(); ++i) {
        data[i] = new BlockFecDatagram;
        data[i]->SetPacket(ans[i]);
    }
    BlockFecCodec codec;
    codec.Encode(data, FecType::FEC_4_2);

    vector<FecDatagram*> lossed_data(ans.size());
    lossed_data[0] = new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(data[0]));
    lossed_data[1] = new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(data[1]));
    lossed_data[2] = new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(data[3]));
    lossed_data[3] = new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(data[5]));
    LOG(ERROR) << codec.Decode(lossed_data, FecType::FEC_4_2);

    for (int i = 0; i < ans.size(); ++i) {
        EXPECT_EQ(*dynamic_cast<BlockFecDatagram*>(lossed_data[i]), *dynamic_cast<BlockFecDatagram*>(data[i]));
    }

}