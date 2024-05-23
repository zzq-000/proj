#include <cstdint>
#include <gtest/gtest.h>
#include "packet.pb.h"
#include "transport/transport_util.h"
#include <google/protobuf/util/message_differencer.h>

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