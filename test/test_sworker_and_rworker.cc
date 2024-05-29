#include <gtest/gtest.h>
#include "transport/sender_worker.h"
#include "transport/receiver_worker.h"
#include "transport/packet.pb.h"
#include "transport/transport_util.h"


TEST(SWorkerAndRWorker, Basic) {
    Config config = Config::GetDefaultConfig();
    config.fec_type = RandomFecType();


    SWorker sworker(config);
    RWorker rworker(config);

    uint64_t start_seq_num = RandomSeqNum();
    std::vector<DataPacket> history;

    FecInfo info = GetInfoAboutFEC(config.fec_type);

    std::list<Packet*> pkts;
    for (int i = 0; i < info.data_cnt; ++i) {
        DataPacket p = RandomDataPacket();
        history.push_back(p);
        sworker.RegisterPackets(p, pkts);
        if (i != info.data_cnt - 1) {
            EXPECT_EQ(pkts.size(), 0);
        }else {
            EXPECT_EQ(pkts.size(), info.TotalCount());
        }
    }
}