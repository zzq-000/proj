#include "gtest/gtest.h"
#include "FecCodec.h"
#include <fstream>
#include <iostream>
                                                    //[0, 100]
std::vector<FecDatagram*> SimulateLoss(const std::vector<FecDatagram*>& data, int loss_rate, FecType t) {
    auto info = GetInfoAboutFEC(t);
    std::vector<FecDatagram*> ans;
    for (int i = 0; i < data.size(); ++i) {

        if (rand() % 101 > loss_rate) {
            auto tmp_ptr = dynamic_cast<BlockFecDatagram*>(data[i]);
            // if (tmp_ptr == NULL) {
            //     // LOG(ERROR) << "error";
            // }
            // if (i < info.data_cnt) {
            //     LOG(INFO) << "outer: " << tmp_ptr->GetPacketPayloadLen();
            // }
            BlockFecDatagram* ptr = new BlockFecDatagram(*tmp_ptr);
            EXPECT_EQ(*ptr, *tmp_ptr);
            ans.push_back(ptr);
        } 
    }
    return ans;
}

bool CheckDataRecoverable(const std::vector<FecDatagram*>& data, FecType t) {
    auto info = GetInfoAboutFEC(t);
    return data.size() >= info.data_cnt;
}

TEST(BlockFecCodec, simple) {
    google::InitGoogleLogging("BlockFecCodec");
    FLAGS_log_dir = "./"; 

    BlockFecCodec codec;
    for (int i = 0; i < 10000; ++i) {
        for (int type = 1; type <= 6; type++) {
            FecType t = static_cast<FecType>(type);
            FecInfo info = GetInfoAboutFEC(t);
            std::vector<FecDatagram*> data; 
            for (int n = 0; n < info.data_cnt; n++) {
                BlockFecDatagram* fecdata = new BlockFecDatagram;
                BlockFecDatagram::RandomBlockFecDatagram(fecdata);
                // LOG(INFO) << static_cast<int>(fecdata->GetPacketPayloadLen());
                data.push_back(fecdata);
            }
            bool res = codec.Encode(data, t);
            // for (int r = 0; r < data.size(); ++r) {
            //     LOG(INFO) << static_cast<int>(data[r]->GetFecIndex());
            // }
            EXPECT_EQ(res, true);

            // std::vector<FecDatagram*> copied_data;
            // for (int k = 0; k < data.size(); ++k) {
            //     BlockFecDatagram* tmp = new BlockFecDatagram(*dynamic_cast<BlockFecDatagram*>(data[k]));
            //     copied_data.push_back(tmp);
            // }
            // for (int k = 0; k < info.data_cnt; ++k) {
            //     EXPECT_EQ(*dynamic_cast<BlockFecDatagram*>(data[k]), *dynamic_cast<BlockFecDatagram*>(copied_data[k]));
            // }
            
            for(int j = 0; j < 6; ++j) {
                auto lossed_data = SimulateLoss(data, j * 10, t);
                for (int r = 0; r < lossed_data.size(); ++r) {
                    int idx = lossed_data[r]->GetFecIndex();
                    auto lossed = dynamic_cast<BlockFecDatagram*>(lossed_data[r]);
                    auto unlossed = dynamic_cast<BlockFecDatagram*>(data[idx]);
                    CHECK_EQ(*lossed, *unlossed) << idx << " " << r << " " << info.data_cnt << " " << info.redundancy_cnt;
                }
                bool real_ans = CheckDataRecoverable(lossed_data, t);
                auto ans = codec.Decode(lossed_data, t);
                // if (ans) {
                //     for (int i = 0 ; i < info.data_cnt; ++i) {
                //         LOG(ERROR) << static_cast<int> (lossed_data[i]->GetFecIndex());
                //     }
                // }
                EXPECT_EQ(real_ans, ans);
                if (ans) {

                //     EXPECT_GE(lossed_data.size(), info.data_cnt);
                //     EXPECT_GE(data.size(), info.data_cnt);
                    for (int q = 0; q < info.data_cnt; ++q) {
                        // LOG(INFO) << static_cast<int>(lossed_data[k]->GetFecIndex());
                        auto lossed = dynamic_cast<BlockFecDatagram*>(lossed_data.at(q));
                        auto unlossed = dynamic_cast<BlockFecDatagram*>(data[q]);
                        EXPECT_EQ(lossed->GetPacketPayloadLen(), unlossed->GetPacketPayloadLen()) << info.data_cnt << " " << info.redundancy_cnt;
                        if (!(*lossed == *unlossed)) {
                            LOG(ERROR) << info.data_cnt << " " << info.redundancy_cnt;
                            std::ofstream file("as.obj", std::ios::binary);
                            for (int tt = 0; tt < info.data_cnt; ++tt) {
                                auto tmp = dynamic_cast<BlockFecDatagram*>(data[tt]);
                                file.write(reinterpret_cast<char*>(tmp), sizeof(Datagram));
                                char ch = '\n';
                                file.write(&ch, 1);
                                // ((Datagram*)(tmp->GetPacket()))->DumpToFile("as.obj");
                            }
                            file.close();
                            exit(0);
                        }
                        // CHECK_EQ(*lossed, *unlossed) << info.data_cnt << " " << info.redundancy_cnt << " " << q;

                    }
                }
            }
        }
    }
    // codec.Encode()
}