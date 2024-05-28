#include <gtest/gtest.h>
#include "FEC/fec_codec.h"
#include "transport/transport_util.h"
#include "transport/packet.pb.h"


TEST(Packet_FEC, Basic_Encode) {
    srand(time(NULL));
    FecType type = RandomFecType();
    FecInfo info = GetInfoAboutFEC(type);

    int data_num = rand() % info.data_cnt;
    int seq_num = 0;

    std::vector<Packet> packets;
    int encode_size = 0;
    for (int i = 0; i < data_num; ++i) {
        Packet p = RandomPacket(seq_num++);
        // GTEST_LOG_(INFO) << p.data_packet().len();
        packets.push_back(p);
        encode_size = std::max(encode_size, (int)packets.at(i).data_packet().ByteSizeLong());
        // GTEST_LOG_(INFO) << packets.at(i).subpacket_len();
    }
    // GTEST_LOG_(INFO) << "======encode_size: " << encode_size << "======";
    for (int i = data_num; i < info.data_cnt; ++i) {
        DataPacket* data = new DataPacket;
        Packet p = RandomPacket(seq_num++);
        p.set_allocated_data_packet(data);
        p.set_subpacket_len(data->ByteSizeLong());
        packets.push_back(p);
    }

    std::vector<void*> data;
    for (int i = 0; i < info.TotalCount(); ++i) {
        void * buffer = new uint8_t[encode_size];
        if (i < info.data_cnt) {
            int size = packets.at(i).data_packet().ByteSizeLong();
            bool ans = packets[i].data_packet().SerializeToArray(buffer, size);
            EXPECT_EQ(ans, true);
        }
        data.push_back(buffer);
    }

    BlockFecCodec codec;


    codec.Encode(data, type, encode_size);

    // for (int i = 0; i < info.TotalCount(); ++i) {
    //     packets.at(i).set_fec_index(i);
    // }
    // GTEST_LOG_(INFO) << "hello";
    int erased = 0;
    int rate = 30;
    for (int i = 0; i < info.TotalCount(); ++i) {
        if (rand() % 100 < 30) {
            delete[] (uint8_t*)data[i];
            data[i] = NULL;
            erased ++;
        }
    }
    if (erased <= info.redundancy_cnt && encode_size) {
        bool decode_ans = codec.Decode(data, type, encode_size);
        EXPECT_EQ(decode_ans, true);
        for (int i = 0; i < info.data_cnt; ++i) {
            DataPacket p;
            p.ParseFromArray(data[i], packets.at(i).data_packet().ByteSizeLong());
            if (i < data_num) {
                EXPECT_EQ(p.DebugString(), packets.at(i).data_packet().DebugString());
                EXPECT_EQ(p.ByteSizeLong(), packets.at(i).data_packet().ByteSizeLong());
            }else {
                // EXPECT_EQ(p.len(), 0);
            }
        }
    }

}