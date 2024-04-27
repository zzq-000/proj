#include <vector>
#include <cstdlib>
#include "cm256/cm256.h"
#include <iostream>
#include <glog/logging.h>
#include <memory>
using namespace std;
using namespace google;
constexpr int dataLen = 998;

enum class Fec_type{
    FEC_2_1,
    FEC_3_1,
    FEC_3_2
};

struct Fec_info{
    int data_cnt;
    int redundancy_cnt;
    std::string type;
};
Fec_info GetInfoAboutFEC(Fec_type t) {
    switch (t)
    {
    case Fec_type::FEC_2_1 :
        return {2, 1, "FEC_2_1"};
    case Fec_type::FEC_3_1 :
        return {3, 1, "FEC_3_1"};
    case Fec_type::FEC_3_2 :
        return {3, 2, "FEC_3_2"};
    default:
        LOG(ERROR) << "no such fec_type";
        return {};
    }
}

struct Datagram{
    uint16_t len;
    uint8_t data[dataLen];
};

class FecEncoder
{
private:
    cm256_block blocks[256];
    cm256_encoder_params params;
public:
    FecEncoder() {
        if (cm256_init()) {
            cout << "init error" << endl;
        }
    };
    void Encode(std::vector<std::shared_ptr<Datagram>>& buffers, Fec_type type) {
        if (buffers.empty()) return;
        Fec_info info = GetInfoAboutFEC(type);
        DCHECK_EQ(buffers.size(), info.data_cnt) << "buffer size is not equal to fec_type.data_cnt";
        params.OriginalCount = buffers.size();
        params.RecoveryCount = info.redundancy_cnt;
        params.BlockBytes = buffers.front()->len;
        for (int i = 0; i < buffers.size(); ++i) {
            // buffers[i].fec_index = cm256_get_original_block_index(params, i);
            blocks[i].Block = buffers.at(i)->data;
            blocks[i].Index = cm256_get_original_block_index(params, i);
            params.BlockBytes = max(params.BlockBytes, (int)buffers.at(i)->len);
        }
        uint8_t* recovery_data = new uint8_t[info.redundancy_cnt * params.BlockBytes];
        cm256_encode(params, blocks, recovery_data);
        for (int i = 0; i < info.redundancy_cnt; ++i) {
            auto ptr_d = std::make_shared<Datagram>();
            ptr_d -> len = params.BlockBytes;
            // d.fec_index = cm256_get_recovery_block_index(params, i);
            memcpy(ptr_d->data, recovery_data + i * params.BlockBytes, params.BlockBytes);
            buffers.push_back(ptr_d);
        }
        return;
    }
    bool Decode(std::vector<std::shared_ptr<Datagram>>& buffers, Fec_type type) {
        
        Fec_info info = GetInfoAboutFEC(type);
        DCHECK_GE(buffers.size(), info.data_cnt) << "buffer size is not greater or equal to fec_type.data_cnt";
        params.BlockBytes = buffers.front()->len;
        for (int i = 0; i < info.data_cnt; ++i) {
            blocks[i].Block = buffers.at(i)->data;
        } 
    }
    ~FecEncoder() {};
};

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, "./");
    google::SetLogFilenameExtension("log_");
    google::InstallFailureSignalHandler();
    FecEncoder f;

    vector<std::shared_ptr<Datagram>> buffer;;
    for (int i = 0; i < 5; ++i) {
        auto ptr = std::make_shared<Datagram>();
        buffer.push_back(ptr);
    }
    f.Encode(buffer, Fec_type::FEC_2_1);
    // 设置日志文件存储位置和文件名前缀
    
    // 设置当日志级别超过某阈值时，程序终止执行
    // google::SETLEve
    // LOG(INFO) << "This is an info message";
    // LOG(WARNING) << "This is a warning message";
    // LOG(ERROR) << "This is an error message";
    // LOG(WARNING) << "This is a warning message";
    // f.Encode({});

}