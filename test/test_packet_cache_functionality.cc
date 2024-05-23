#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/message_differencer.h>
#include <vector>
#include "transport/packet.pb.h"
#include "transport/packet_cache.h"
#include "transport/transport_util.h"


TEST(PacketCache, PacketSizeLECacheSize) {
    constexpr int kCacheSize = 1000;

    PacketCache<Packet> cache(kCacheSize);

    constexpr int kPacketsNumber = kCacheSize;

    std::vector<Packet> original_packets;
    original_packets.reserve(kPacketsNumber);
    
    uint64_t start_seq = RandomSeqNum();

    for (int i = 0; i < kPacketsNumber; ++i) {
        Packet p = RandomPacket(start_seq);
        original_packets.push_back(p);
        cache.CachePacket(p);
        EXPECT_EQ(p.seq_num(), start_seq);
        start_seq++;
    }
    for (int i = 0; i < kPacketsNumber; ++i) {
        uint64_t seq_num = original_packets[i].seq_num();
        Packet* p = cache.FindPacket(seq_num);
        EXPECT_EQ(p != NULL, true) ;
        google::protobuf::util::MessageDifferencer::Equals(original_packets.at(i), *p);

    }
    // LOG(INFO) << p.ByteSizeLong();
}

TEST(PacketCache, PacketSizeGTCacheSize) {
    constexpr int kCacheSize = 1000;

    PacketCache<Packet> cache(kCacheSize);

    constexpr int kPacketsNumber = kCacheSize * 10;

    std::vector<Packet> original_packets;
    original_packets.reserve(kPacketsNumber);
    
    uint64_t start_seq = RandomSeqNum();

    for (int i = 0; i < kPacketsNumber; ++i) {
        Packet p = RandomPacket(start_seq);
        original_packets.push_back(p);
        cache.CachePacket(p);
        EXPECT_EQ(p.seq_num(), start_seq);
        start_seq++;
    }
    for (int i = 0; i < kPacketsNumber; ++i) {
        uint64_t seq_num = original_packets[i].seq_num();
        Packet* p = cache.FindPacket(seq_num);
        if (i < kPacketsNumber - kCacheSize) {
            EXPECT_EQ(p != NULL, false);
        }else {
            EXPECT_EQ(p != NULL, true);
            google::protobuf::util::MessageDifferencer::Equals(original_packets.at(i), *p);
        }
    }
}