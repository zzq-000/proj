#include <gtest/gtest.h>
#include <packet.pb.h>
#include <transport/packet_logger.h>
#include <transport/transport_util.h>
#include <util/timestamp.h>
TEST(PacketLogger, Basic) {
    PacketLogger logger;
    uint64_t start_seq_num = RandomSeqNum();
    int packets_num = 10000;
    uint64_t now = timestamp_ms();
    for (int i = 0; i < packets_num; ++i) {
        Packet p = RandomPacket(start_seq_num++);
        if (rand() % 100 < 30) {
            continue;
        }
        logger.ReceivePacket(p, 0);
    }
}