#include <gtest/gtest.h>
#include <google/protobuf/util/message_differencer.h>
#include "transport/sender_worker.h"
#include "transport/transport_util.h"

/*如果packet_cache设置得过小， 则会导致测试用例不通过， 
因为std::list<Packtet*>里面的元素都是指向packet_cache里面的指针
一旦std::list的大小超过packet_cache大小， 则会导致部分指针失效*/

TEST(SWorker, RegisterPackets_without_FEC) {
    srand(time(NULL));

    Config config = Config::GetDefaultConfig();
    SWorker worker(config);

    uint64_t seq_num = RandomSeqNum();

    int kPacketNum = 10000;
    std::vector<DataPacket> history;
    history.reserve(kPacketNum);
    std::list<Packet*> rtn;
    for (int i = 0; i < kPacketNum; ++i) {
        rtn.clear();
        DataPacket d = RandomDataPacket();
        history.push_back(d);
        worker.RegisterPackets(d, rtn);

        EXPECT_EQ(rtn.size(), 1);

        Packet* p = rtn.front();

        EXPECT_EQ(p->seq_num(), i);
        EXPECT_EQ(p->fec_type(), config.fec_type);
        EXPECT_EQ(google::protobuf::util::MessageDifferencer::Equals(p->data_packet(), history.at(i)), true);
    }
}

// 测试Sworker生成的包是否正确， 是否可以被解码
TEST(SWorker, RegisterPackets_with_FEC_redundant_packets) {
    srand(time(NULL));
    Config config = Config::GetDefaultConfig();
    config.fec_type = RandomFecType();


    SWorker sworker(config);

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
    int encode_size = 0;
    std::vector<Packet*> cache;
    int cnt = 0;
    for (Packet* p : pkts) {
        cache.push_back(p);
        if (cnt < info.data_cnt) {
            encode_size = std::max(encode_size, (int)(p->data_packet().ByteSizeLong()));
            EXPECT_EQ(p->data_packet().DebugString(), history.at(cnt++).DebugString());
        }
    }
    // GTEST_LOG_(INFO) << info.type << " " << pkts.size();
    std::vector<void*> data(info.TotalCount());
    uint8_t* buffer = new uint8_t[info.TotalCount() * encode_size];
    memset(buffer, 0, info.TotalCount() * encode_size);
    cnt = 0;
    BlockFecCodec codec;

    for (int i = 0; i < info.TotalCount(); ++i) {
        data[i] = buffer + i * encode_size;
        if (i < info.data_cnt) {
            cache[i]->data_packet().SerializeToArray(data[i], cache[i]->data_packet().ByteSizeLong());
            // history[i].SerializeToArray(data[i], history[i].ByteSizeLong());
        }
    }
    codec.Encode(data, config.fec_type, encode_size);
    
    for (int i = info.data_cnt; i < info.TotalCount(); ++i) {
        std::string s = cache[i]->data_packet().data();
        EXPECT_EQ(s.size(), encode_size);

        EXPECT_EQ(memcmp(data[i], s.data(), encode_size), 0);
    }


    for (int i = 0; i < info.redundancy_cnt; ++i) {
        data[i] = NULL;
    }
    codec.Decode(data, config.fec_type, encode_size);
    for (int i = 0; i < info.data_cnt; ++i) {
        DataPacket p;
        p.ParseFromArray(data[i], history[i].ByteSizeLong());
        EXPECT_EQ(p.DebugString(), history[i].DebugString());
    }
    delete[] buffer;
}

TEST(SWorker, RegisterPackets_with_FEC_data_packets) {
    Config config = Config::GetDefaultConfig();
    config.fec_type = RandomFecType();
    FecInfo info = GetInfoAboutFEC(config.fec_type);

    // GTEST_LOG_(INFO) << info.type;

    SWorker worker(config);


    // uint64_t seq_num = RandomSeqNum();

    int kPacketNum = 1000 * info.TotalCount();

    std::vector<DataPacket> history;
    history.reserve(kPacketNum);
    int history_index = 0;
    int seq_num = 0;
    std::list<Packet*> rtn;
    for (int i = 0; i < kPacketNum; ++i) {
        rtn.clear();
        DataPacket d = RandomDataPacket();
        history.push_back(d);
        // GTEST_LOG_(INFO) << i << " " << info.type << " " << d.ByteSizeLong();
        worker.RegisterPackets(d, rtn);

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

TEST(SWorker, RegisterPackets_with_FEC_decode) {
    Config config = Config::GetDefaultConfig();
    config.fec_type = RandomFecType();

    SWorker sworker(config);

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
    int encode_size = 0;
    std::vector<Packet*> cache;
    int cnt = 0;
    for (Packet* p : pkts) {
        cache.push_back(p);
        if (cnt < info.data_cnt) {
            encode_size = std::max(encode_size, (int)(p->data_packet().ByteSizeLong()));
            EXPECT_EQ(p->data_packet().DebugString(), history.at(cnt++).DebugString());
        }
    }
    // GTEST_LOG_(INFO) << info.type << " " << pkts.size();
    std::vector<void*> data(info.TotalCount());
    uint8_t* buffer = new uint8_t[info.TotalCount() * encode_size];
    memset(buffer, 0, info.TotalCount() * encode_size);
    cnt = 0;
    BlockFecCodec codec;

    for (int i = 0; i < info.TotalCount(); ++i) {
        data[i] = buffer + i * encode_size;
        if (i < info.data_cnt) {
            cache[i]->data_packet().SerializeToArray(data[i], cache[i]->data_packet().ByteSizeLong());
        }else {
            memcpy(data[i], cache[i]->data_packet().data().data(), cache[i]->data_packet().data().size());
        }
    }
    // codec.Encode(data, config.fec_type, encode_size);

    for (int i = 0; i < info.redundancy_cnt; ++i) {
        data[i] = NULL;
    }
    codec.Decode(data, config.fec_type, encode_size);
    for (int i = 0; i < info.data_cnt; ++i) {
        DataPacket p;
        p.ParseFromArray(data[i], history[i].ByteSizeLong());
        EXPECT_EQ(p.DebugString(), history[i].DebugString());
    }
}

TEST(SWorker, Change_FecType) {
    srand(3);
    Config config = Config::GetDefaultConfig();
    config.fec_type = FecType::FEC_NONE;


    SWorker sworker(config);

    uint64_t start_seq_num = RandomSeqNum();
    std::vector<DataPacket> history;
    // rand();
    int fec_none_packets = rand() % 1000 + 100;
    // int mean = rand() % 1000 + 100;
    // int fec_none_packets = 100;

    // GTEST_LOG_(INFO) << "fec_none_packets: " << fec_none_packets;
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
    // GTEST_LOG_(INFO) << "fec_type: " << info.type << ", fec packets: "<< fec_type_packets;

    for (int i = 0; i < info.data_cnt; ++i) {
        DataPacket p = RandomDataPacket();
        history.push_back(p);
        sworker.RegisterPackets(p, pkts);
    }
    sworker.ClearFec(pkts);
    EXPECT_EQ((pkts.size() - fec_none_packets) % info.TotalCount(), 0);
    // GTEST_LOG_(INFO) << pkts.size() - fec_none_packets;
    cnt = 0;
    for (Packet* p : pkts) {
        if (cnt < fec_none_packets) {
            EXPECT_EQ(p->fec_type(), FecType::FEC_NONE);
            EXPECT_EQ(p->data_packet().DebugString(), history.at(cnt++).DebugString());
        }else {
            EXPECT_EQ(p->fec_type(), type);
            if (p->fec_index() < info.data_cnt) {
                if (cnt < fec_none_packets + info.data_cnt) {
                    EXPECT_EQ(p->data_packet().DebugString(), history.at(cnt++).DebugString());  
                }else {
                    DataPacket tmp;
                    EXPECT_EQ(p->data_packet().DebugString(), tmp.DebugString());
                }
            }
        }
    }

}