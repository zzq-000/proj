#include <unordered_set>
#include <fstream>
#include <iostream>
#include "FecCodec.h"
#include "fec_type.pb.h"
#include "FecDatagram.h"
#include "gtest/gtest.h"

TEST(BlockFecCodec, Basic) {
    BlockFecCodec codec;
    FecType type = FecType::FEC_3_1;
    int len = 100;
    auto info = GetInfoAboutFEC(type);
    std::vector<void*> original_data(info.data_cnt + info.redundancy_cnt);
    for (int k = 0; k < info.data_cnt + info.redundancy_cnt; ++k) {
        original_data[k] = (void*)(new uint8_t[len]);
    }
    for (int k = 0; k < info.data_cnt; ++k) {
        for (int r = 0; r < len; ++r) {
            ((uint8_t*)(original_data[k]))[r] = rand() % UINT8_MAX;
        }
    }
    codec.Encode(original_data, type, len);

    // int cnt = 0;
    std::vector<void*> lossed_data(info.data_cnt + info.redundancy_cnt);
    for (int i = 0; i < info.data_cnt + info.redundancy_cnt; ++i) {
        lossed_data[i] = new uint8_t[len];
        memcpy(lossed_data[i], original_data[i], len);
    }
    lossed_data[0] = NULL;



    bool ans = codec.Decode(lossed_data, type, len);
    // EXPECT_EQ(real_ans, ans);
    if (ans) {
        for (int i = 0; i < info.data_cnt; ++i) {
            EXPECT_EQ(memcmp(original_data[i], lossed_data[i], len), 0);
        }
    }
}


struct Stat {
    FecType type;
    const int loss_rate;      // 设定的丢包率
    const int packet_size;
    int data_packets;         // 真实数据包
    int fec_packets;          // 冗余数据包
    int lossed_data_packets;  // 丢失的真实数据包
    int lossed_fec_packets;   // 丢失的冗余数据包
    int recoveried_packets;   // 恢复的真实数据包
    Stat(FecType t, int loss_rate, int packet_size): type(t), loss_rate(loss_rate), packet_size(packet_size),
        data_packets(0), fec_packets(0), lossed_data_packets(0), 
        lossed_fec_packets(0),recoveried_packets(0)  {

    }

    inline std::string Str() const {
        std::string rtn;
        FecInfo info = GetInfoAboutFEC(type);
        rtn = "FecType: " + info.type
            + "; loss rate: " + std::to_string(loss_rate) 
            + "; packet size: " + std::to_string(packet_size) 
            + "; data packets: " + std::to_string(data_packets)
            + "; fec packets: " + std::to_string(fec_packets)
            + "; lossed data packets: " + std::to_string(lossed_data_packets)
            + "; lossed fec packets: " + std::to_string(lossed_fec_packets)
            + "; recoveried packets: " + std::to_string(recoveried_packets)
            + "\n";
        return rtn;
    }
};


TEST(BlockFecCodec, Advance) {
// int main() {
    BlockFecCodec codec;
    std::unordered_set<int> blockbytes{100, 1000, 1472};
    std::unordered_set<int> loss_rates{0, 10, 20, 30, 40};
    std::vector<Stat> stats_collection;
    int repeat = 1000;
    for (int i = 1; i < 16; ++i) {
        FecType type = static_cast<FecType>(i);
        auto info = GetInfoAboutFEC(type);
        for (int loss : loss_rates) {


            for (int length : blockbytes) {
                Stat stat(type, loss, length); //

                for (int j = 0; j < repeat; ++j) {
                    // std::cout << info.type << " " << loss << " " << length << std::endl;
                    stat.data_packets += info.data_cnt;
                    stat.fec_packets += info.redundancy_cnt;


                    std::vector<void*> original_data(info.TotalCount());
                    for (int k = 0; k < info.data_cnt + info.redundancy_cnt; ++k) {
                        original_data[k] = (void*)(new uint8_t[length]);
                    }
                    for (int k = 0; k < info.data_cnt; ++k) {
                        for (int r = 0; r < length; ++r) {
                            ((uint8_t*)(original_data[k]))[r] = rand() % UINT8_MAX;
                        }
                    }
                    codec.Encode(original_data, type, length);
                    int cnt = 0;
                    int loss_data_cnt = 0;
                    std::vector<void*> lossed_data(info.TotalCount());

                    for (int i = 0; i < info.TotalCount(); ++i) {
                        lossed_data[i] = new uint8_t[length];
                        memcpy(lossed_data[i], original_data[i], length);
                        if (rand() % 100 < loss) {
                            delete[] (uint8_t*)lossed_data[i];
                            lossed_data[i] = NULL;

                            if (i < info.data_cnt) {
                                stat.lossed_data_packets++; //
                                loss_data_cnt++;
                            }else {
                                stat.lossed_fec_packets++;  //
                            }

                            cnt++;
                        }
                    }
                    bool real_ans = (cnt <= info.redundancy_cnt);
                    bool ans = false;
                    if (real_ans) {

                        ans = codec.Decode(lossed_data, type, length);

                        stat.recoveried_packets += loss_data_cnt;  //
                        
                        // EXPECT_EQ(real_ans, ans);
                    }
                    if (ans) {
                        for (int i = 0; i < info.data_cnt; ++i) {
                            // EXPECT_EQ(memcmp(original_data[i], lossed_data[i], length), 0);
                        }
                    }

                }
                stats_collection.push_back(stat);
            }
        }
    }
    std::ofstream file("log.data");
    for (Stat& stat : stats_collection) {
        file << stat.Str();
    }
}

