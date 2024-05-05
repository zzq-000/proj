#include "gtest/gtest.h"
#include "FecCodec.h"
#include "glog/logging.h"
                                                    //[0, 100]
std::vector<FecDatagram*> SimulateLoss(const std::vector<FecDatagram*>& data, int loss_rate) {
    std::vector<FecDatagram*> ans;
    for (int i = 0; i < data.size(); ++i) {
        if (rand() % 101 > loss_rate) {
            ans.push_back(data[i]);
        } 
    }
    return ans;
}

bool CheckDataRecoverable(const std::vector<FecDatagram*>& data, Fec_type t) {
    auto info = GetInfoAboutFEC(t);
    return data.size() >= info.data_cnt;
}

TEST(BlockFecCodec, simple) {
    BlockFecCodec codec;
    for (int i = 0; i < 10000; ++i) {
        for (int type = 1; type <= 6; type++) {
            Fec_type t = static_cast<Fec_type>(type);
            Fec_info info = GetInfoAboutFEC(t);
            std::vector<FecDatagram*> data; 
            for (int n = 0; n < info.data_cnt; n++) {
                BlockFecDatagram* fecdata = new BlockFecDatagram;
                BlockFecDatagram::RandomBlockFecDatagram(fecdata);
                data.emplace_back(fecdata);
            }
            bool res = codec.Encode(data, t);
            EXPECT_EQ(res, true);

            for(int j = 0; j < 10; ++j) {
                auto lossed_data = SimulateLoss(data, j * 10);
                bool real_ans = CheckDataRecoverable(lossed_data, t);

                auto ans = codec.Decode(lossed_data, t);
                EXPECT_EQ(real_ans, ans);
            }
        }
    }
    // codec.Encode()
}