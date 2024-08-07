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
    int cnt = 0;
    for (Packet* p : pkts) {
        if (cnt < info.data_cnt) {
            EXPECT_EQ(p->data_packet().DebugString(), history.at(cnt++).DebugString());
        }
    }
    // GTEST_LOG_(INFO) << info.type;

    std::list<DataPacket*> messages;

    int loss = 0;
    cnt = 0;
    int loss_rate = 100;
    for (Packet *p : pkts) {
        // GTEST_LOG_(INFO) << p->fec_index() << " " << p->data_packet().ByteSizeLong();
        if (cnt < info.data_cnt) {
            if (loss < info.redundancy_cnt) {
                if (rand() % 100 < loss_rate) {
                    loss++;
                    cnt++;
                    continue;
                }
            }
        }
        rworker.GetApplicationMessages(*p, messages);
        cnt++;
    }
    EXPECT_GT(loss, 0);
    EXPECT_EQ(messages.size(), info.data_cnt);
    cnt = 0;
    for (DataPacket* p : messages) {
        EXPECT_EQ(history.at(cnt).DebugString(), p->DebugString()) << cnt;
        cnt++;
    }
    // for (int i = 0; i < info.data_cnt; ++i) {
    //     EXPECT_EQ(history.at(i).DebugString(), mess)
    // }
    // GTEST_LOG_(INFO) << loss << " " << messages.size();
}


TEST(SWorkerAndRWorker, Change_FecType1) {
    srand(time(NULL));
    Config config = Config::GetDefaultConfig();
    config.fec_type = FecType::FEC_NONE;


    SWorker sworker(config);
    RWorker rworker(config);

    uint64_t start_seq_num = RandomSeqNum();
    std::vector<DataPacket> history;

    int fec_none_packets = rand() % 1000 + 100;

    GTEST_LOG_(INFO) << "fec_none_packets: " << fec_none_packets;
    std::list<Packet*> pkts;
    for (int i = 0; i < fec_none_packets; ++i) {
        DataPacket p = RandomDataPacket();
        history.push_back(p);
        sworker.RegisterPackets(p, pkts);
        EXPECT_EQ(pkts.size(), i + 1);
    }
    int cnt = 0;
    for (Packet* p : pkts) {
        if (cnt < fec_none_packets) {
            EXPECT_EQ(p->data_packet().DebugString(), history.at(cnt++).DebugString());
        }
    }
    // GTEST_LOG_(INFO) << info.type;
    FecType type = RandomFecType();
    config.fec_type = type;
    sworker.SetConfig(config);
    FecInfo info = GetInfoAboutFEC(type);
    int fec_type_packets = rand() % 1000 + 100;
    GTEST_LOG_(INFO) << "fec_type: " << info.type << ", fec packets: "<< fec_type_packets;

    for (int i = 0; i < fec_type_packets; ++i) {
        DataPacket p = RandomDataPacket();
        history.push_back(p);
        sworker.RegisterPackets(p, pkts);
    }
    sworker.ClearFec(pkts);
    EXPECT_EQ((pkts.size() - fec_none_packets) % info.TotalCount(), 0);
    cnt = 0;
    for (Packet* p : pkts) {
        if (cnt < fec_none_packets) {
            EXPECT_EQ(p->fec_type(), FecType::FEC_NONE);
            EXPECT_EQ(p->data_packet().DebugString(), history.at(cnt++).DebugString());
        }else {
            EXPECT_EQ(p->fec_type(), type);
            if (p->fec_index() < info.data_cnt) {
                if (cnt < fec_none_packets + fec_type_packets) {
                    EXPECT_EQ(p->data_packet().DebugString(), history.at(cnt++).DebugString());  
                }else {
                    DataPacket tmp;
                    EXPECT_EQ(p->data_packet().DebugString(), tmp.DebugString());
                }
            }
        }
    }

}
