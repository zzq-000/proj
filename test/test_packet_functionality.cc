#include <cstdint>
#include <gtest/gtest.h>
#include "packet.pb.h"
#include "transport/transport_util.h"
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/message.h>


TEST(Packet, BasicCopy) {
    uint64_t seq_num = RandomSeqNum();
    Packet p = RandomPacket(seq_num);

    Packet r;
    r.CopyFrom(p);
    EXPECT_EQ(google::protobuf::util::MessageDifferencer::Equals(r, p), true);
    EXPECT_EQ(r.SerializeAsString(), p.SerializeAsString());
    EXPECT_EQ(r.DebugString(), p.DebugString());
}

TEST(DataPacket, Clear) {
    DataPacket d;
    // d.Clear();
    EXPECT_EQ(d.ByteSizeLong(), 0);
}

TEST(DataPacket, Set_data) {
    DataPacket d;
    uint8_t buffer[100];
    d.set_data(buffer, 100);
}

TEST(Vector, pointer) {
    int len = 7;
    std::vector<void*> buffer(len);
    int size = 1000;
    for (int i = 0; i < len; ++i) {
        buffer[i] = new uint8_t[size];
    }

    for (int i = 0; i < len; ++i) {
        // delete[] (uint8_t*)buffer[i];
        delete[] static_cast<uint8_t*>(buffer[i]);
    }

}

TEST(Packet, Const_reference) {
    Packet p = RandomPacket(0);
    // GTEST_LOG_(INFO) << p.subpacket_len();
    const_cast<DataPacket&>(p.data_packet()).set_len(100);
    // GTEST_LOG_(INFO) << p.data_packet().len();
    EXPECT_EQ(p.data_packet().len(), 100);

}

TEST(Packet, Const_pointer) {
    Packet p = RandomPacket(0);
    // GTEST_LOG_(INFO) << p.subpacket_len();
    const_cast<DataPacket*>(&(p.data_packet()))->set_len(100);
    EXPECT_EQ(p.data_packet().len(), 100);
    // GTEST_LOG_(INFO) << p.data_packet().len();
}
TEST(DataPacket, serialize_and_deserialize) {
    srand(time(NULL));
    for (int i = 0; i < 20000; ++i) {
        DataPacket p = RandomDataPacket();
        uint8_t buffer[1500];
        // memset(buffer, 0, sizeof buffer);
        p.SerializeToArray(buffer, p.ByteSizeLong());
        // GTEST_LOG_(INFO) << p.ByteSizeLong();
        google::protobuf::io::ArrayInputStream array_input_stream(buffer, 1500);
        google::protobuf::io::CodedInputStream coded_input_stream(&array_input_stream);

        uint32_t len = 0;
        coded_input_stream.ReadTag();
        coded_input_stream.ReadVarint32(&len);
        EXPECT_EQ(p.len(), p.ByteSizeLong());
        EXPECT_EQ(len, p.len());
        EXPECT_EQ(len, p.ByteSizeLong());
        DataPacket rhs;
        rhs.ParseFromArray(buffer, len);
        EXPECT_EQ(p.DebugString(), rhs.DebugString());

    }
}