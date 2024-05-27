#include <gtest/gtest.h>
#include <google/protobuf/util/message_differencer.h>
#include "transport/sender_worker.h"
#include "transport/transport_util.h"


TEST(SWorker, DISABLED_FEC_NONE) {
    Config config = Config::GetDefaultConfig();
    SWorker worker(config);

    uint64_t seq_num = RandomSeqNum();

    int kPacketNum = 1000;
    std::vector<DataPacket> history;
    history.reserve(kPacketNum);
    for (int i = 0; i < kPacketNum; ++i) {
        DataPacket d = RandomDataPacket();
        history.push_back(d);
        auto rtn = worker.RegisterPackets(d);

        EXPECT_EQ(rtn.size(), 1);

        Packet* p = rtn.front();

        EXPECT_EQ(p->seq_num(), i);
        EXPECT_EQ(p->fec_type(), config.fec_type);
        EXPECT_EQ(google::protobuf::util::MessageDifferencer::Equals(p->data_packet(), history.at(i)), true);
    }
}

TEST(SWorker, FEC) {
    Config config = Config::GetDefaultConfig();
    config.fec_type = RandomFecType();
    FecInfo info = GetInfoAboutFEC(config.fec_type);

    GTEST_LOG_(INFO) << info.type;

    SWorker worker(config);


    // uint64_t seq_num = RandomSeqNum();

    int kPacketNum = 1000 * info.TotalCount();

    std::vector<DataPacket> history;
    history.reserve(kPacketNum);
    int history_index = 0;
    int seq_num = 0;
    for (int i = 0; i < kPacketNum; ++i) {
        DataPacket d = RandomDataPacket();
        history.push_back(d);
        // GTEST_LOG_(INFO) << i << " " << info.type << " " << d.ByteSizeLong();
        auto rtn = worker.RegisterPackets(d);

        EXPECT_EQ(rtn.size() % info.TotalCount(), 0);
        if (rtn.size()) {
            EXPECT_EQ(rtn.size(), info.TotalCount());
            int cnt = 0;
            for (Packet* p : rtn) {
                EXPECT_EQ(p->fec_index(), cnt) << cnt;
                EXPECT_EQ(p->seq_num(), seq_num++) << seq_num;
                if (cnt < info.data_cnt) {
                    EXPECT_EQ(google::protobuf::util::MessageDifferencer::Equals(p->data_packet(), history.at(history_index)), true) << history_index;
                    history_index++;
                }else {
                    EXPECT_GE(p->fec_index(), info.data_cnt);
                }
                cnt++;
            }
        }

    }
}
