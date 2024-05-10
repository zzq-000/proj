
#include "gtest/gtest.h"
#include "Datagram.h"
#include "FecDatagram.h"
#include "FecCodec.h"
#include <glog/logging.h>
#include "PacketBuffer.h"
using namespace std;



/*
-------------
|fec_index  |  
---------------------------            为了节省fec开销，每次编解码时，不以最大长度dataLen作为params.BlockBytes, 
|payload_len|   uint_16   ^            而是包组里面最大的 payload_len + sizeof(uint_16)
-------------             encode 
|           |             region       为了支持变长编解码，需要将payload_len也放在恢复区域，这样数据恢复出来之后可以
|  payload  |                          知道有效数据的真实长度是怎样的
|           |             v
---------------------------

*/





void PrintFecDatagram(FecDatagram* d) {
    cout << "fec_index: " << static_cast<int>(d->GetFecIndex()) << endl;
    cout << "len: " << d->GetPacketPayloadLen() << endl;
    cout << "content: ";
    for (size_t i = 0; i < d->GetPacketPayloadLen(); ++i) {
        cout << ((char*)d->GetPacketPayload())[i];
    }
    cout << endl << endl;;
}


TEST(BlockFecCodec, basic) {

    google::InitGoogleLogging("basic");
    BlockFecCodec f;

    const char* s1 = "hello, world";
    BlockFecDatagram d1(s1, strlen(s1));


    // memcpy(d1.data, s1, strlen(s1));
    // d1.SetLen(strlen(s1));

    const char* s2 = "thank you";
    BlockFecDatagram d2(s2, strlen(s2));

    const char* s3 = "thank you cwenvioevbnoiwenve";
    BlockFecDatagram d3(s3, strlen(s3));

    const char* s4 = "thandvwinwek you cwenvioevbnoiwenve";
    BlockFecDatagram d4(s4, strlen(s4));


    vector<FecDatagram*> buffer;
    buffer.emplace_back(&d1);
    buffer.emplace_back(&d2);
    buffer.emplace_back(&d3);
    buffer.emplace_back(&d4);
    f.Encode(buffer, FecType::FEC_4_2);

    BlockFecDatagram* n1 =  new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(buffer[0]));
    BlockFecDatagram* n2 =  new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(buffer[1]));
    BlockFecDatagram* n3 =  new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(buffer[3]));
    BlockFecDatagram* n4 =  new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(buffer[5]));
    EXPECT_EQ(*n1, *dynamic_cast<BlockFecDatagram*>(buffer[0]));
    EXPECT_EQ(*n2, *dynamic_cast<BlockFecDatagram*>(buffer[1]));
    EXPECT_EQ(*n3, *dynamic_cast<BlockFecDatagram*>(buffer[3]));
    EXPECT_EQ(*n4, *dynamic_cast<BlockFecDatagram*>(buffer[5]));
    vector<FecDatagram*> new_buffer{n1, n2, n3, n4};
    // PrintFecDatagram(buffer[0]);
    // PrintFecDatagram(buffer[2]);
    // LOG(ERROR) << new_buffer.size();
    // for (int i = 0; i < 3;++i) {
    //     LOG(ERROR) << static_cast<int> (new_buffer[i]->GetFecIndex());
    //     PrintFecDatagram(new_buffer[i]);
    //     // EXPECT_EQ(*dynamic_cast<BlockFecDatagram*>(new_buffer[i]), *dynamic_cast<BlockFecDatagram*>(buffer[i])) << i;
    // }
    LOG(ERROR) << f.Decode(new_buffer, FecType::FEC_4_2);
    LOG(ERROR) << new_buffer.size();
    for (int i = 0; i < 4;++i) {
        LOG(ERROR) << static_cast<int> (new_buffer[i]->GetFecIndex());
        PrintFecDatagram(new_buffer[i]);
        // EXPECT_EQ(*dynamic_cast<BlockFecDatagram*>(new_buffer[i]), *dynamic_cast<BlockFecDatagram*>(buffer[i])) << i;
    }
    for (int i = 0; i < 4;++i) {
        // PrintFecDatagram(buffer[i]);
        EXPECT_EQ(*dynamic_cast<BlockFecDatagram*>(new_buffer[i]), *dynamic_cast<BlockFecDatagram*>(buffer[i])) << i;
    }
}

TEST(BlockFecCodec, basic2) {
    PacketBuffer pb;

}