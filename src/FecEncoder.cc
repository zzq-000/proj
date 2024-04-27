#include <vector>
#include <cstdlib>
#include "cm256/cm256.h"
#include <iostream>
#include <glog/logging.h>
#include <arpa/inet.h>
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
    // uint16_t len;
    uint16_t fec_index;
    uint8_t data[dataLen]; // **前两个byte存放长度  2^16 =65536** , 网络传输时需要htons
    Datagram () {}
    Datagram(const uint8_t* buffer, uint16_t len) {
        SetLen(len);
        memcpy(GetData(true), buffer, len);
    }
    Datagram(const char* buffer, uint16_t len) {
        SetLen(len);
        memcpy(GetData(true), buffer, len);
    }
    static Datagram* HostToNetwork(Datagram* d) {
        d->SetFecIndex(htons(d->GetFecIndex()));
        d->SetLen(htons(d->GetLen()));
        return d;
    } 
    static Datagram* NetworkToHost(Datagram* d) {
        d->SetFecIndex(ntohs(d->GetFecIndex()));
        d->SetLen(ntohs(d->GetLen()));
        return d;
    } 

    inline uint16_t GetFecIndex() const {
        return fec_index;
    }
    inline void SetFecIndex(uint16_t fec_index) {
        this->fec_index = fec_index;
    }
    inline uint16_t GetLen() const {
        return *(uint16_t*)(data);
    }
    inline void SetLen(uint16_t len) {
        *(uint16_t*)(data) = len;
    }
    inline const uint8_t* GetData(bool real_data) const {
        return (uint8_t*)data + (real_data ? 2 : 0);

    }
    inline uint8_t* GetData(bool real_data) {
        return (uint8_t*)data + (real_data ? 2 : 0);
    }

};

void PrintDatagram(Datagram* d) {
    cout << "fec_index: " << static_cast<int>(d->fec_index) << endl;
    cout << "len: " << d->GetLen() << endl;
    cout << "content: ";
    for (size_t i = 0; i < d->GetLen(); ++i) {
        cout << ((char*)d->GetData(true))[i];
    }
    cout << endl << endl;;
}

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
    void Encode(std::vector<Datagram*>& buffers, Fec_type type) {
        if (buffers.empty()) return;
        Fec_info info = GetInfoAboutFEC(type);
        DCHECK_EQ(buffers.size(), info.data_cnt) << "buffer size is not equal to fec_type.data_cnt";
        params.OriginalCount = buffers.size();
        params.RecoveryCount = info.redundancy_cnt;
        params.BlockBytes = dataLen;
        for (int i = 0; i < buffers.size(); ++i) {
            // buffers[i].fec_index = cm256_get_original_block_index(params, i);
            blocks[i].Block = buffers.at(i)->GetData(false);
            blocks[i].Index = cm256_get_original_block_index(params, i);
            buffers[i]->fec_index = blocks[i].Index;
            // params.BlockBytes = max(params.BlockBytes, (int)buffers.at(i)->GetLen());
        }
        // LOG(ERROR) << "hello";
        uint8_t* recovery_data = new uint8_t[info.redundancy_cnt * (dataLen + 2)];
        cm256_encode(params, blocks, recovery_data);
        LOG(ERROR) << "hello";
        LOG(ERROR) << info.redundancy_cnt;
        for (int i = 0; i < info.redundancy_cnt; ++i) {
            auto ptr_d = new Datagram;
        // LOG(ERROR) << "hello";

            ptr_d->fec_index = cm256_get_recovery_block_index(params, i);
            memcpy(ptr_d->GetData(false), recovery_data + i * params.BlockBytes, params.BlockBytes);
            buffers.push_back(ptr_d);
        }
        // LOG(ERROR) << "hello";

        delete[] recovery_data;
        return;
    }
    bool Decode(std::vector<Datagram*>& buffers, Fec_type type) {
        
        Fec_info info = GetInfoAboutFEC(type);
        DCHECK_GE(buffers.size(), info.data_cnt) << "buffer size is not greater or equal to fec_type.data_cnt";
        params.BlockBytes = dataLen;
        for (int i = 0; i < info.data_cnt; ++i) {
            blocks[i].Block = buffers.at(i)->GetData(false);
            blocks[i].Index = buffers.at(i)->fec_index;
        } 
        return cm256_decode(params, blocks);
    }
    ~FecEncoder() {};
};

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::INFO, "./");
    google::SetLogFilenameExtension("log_");
    google::InstallFailureSignalHandler();
    FecEncoder f;

    // vector<std::shared_ptr<Datagram>> buffer;
    // for (int i = 0; i < 5; ++i) {
    //     auto ptr = std::make_shared<Datagram>();
    //     buffer.push_back(ptr);
    // }
    // f.Encode(buffer, Fec_type::FEC_2_1);
    const char* s1 = "hello, world";
    Datagram d1(s1, strlen(s1));


    // memcpy(d1.data, s1, strlen(s1));
    // d1.SetLen(strlen(s1));

    const char* s2 = "thank you";
    Datagram d2(s2, strlen(s2));
    // memcpy(d2.data, s2, strlen(s2));
    // d2.len = strlen(s2);

    vector<Datagram*> buffer;
    buffer.emplace_back(&d1);
    buffer.emplace_back(&d2);
    LOG(ERROR) << buffer.size();
    f.Encode(buffer, Fec_type::FEC_2_1);
    LOG(ERROR) << buffer.size();
    // for (int i = 0; i < buffer.size(); ++i) {
    //     PrintDatagram(buffer.at(i));
    // }
    // LOG(ERROR) << static_cast<int>(buffer[0]->fec_index);
    // LOG(ERROR) << static_cast<int>(buffer[1]->fec_index);
    // LOG(ERROR) << static_cast<int>(buffer[2]->fec_index);
    vector<Datagram*> new_buffer{buffer[0], buffer[2]};
    // for (int i = 0; i < new_buffer.size(); ++i) {
    //     PrintDatagram(new_buffer.at(i));
    // }
    LOG(ERROR) << f.Decode(new_buffer, Fec_type::FEC_2_1);

    for (int i = 0; i < new_buffer.size(); ++i) {
        // new_buffer[i]->SetLen(buffer[i]->len;
        PrintDatagram(new_buffer.at(i));
    }

    // 设置日志文件存储位置和文件名前缀
    
    // 设置当日志级别超过某阈值时，程序终止执行
    // google::SETLEve
    // LOG(INFO) << "This is an info message";
    // LOG(WARNING) << "This is a warning message";
    // LOG(ERROR) << "This is an error message";
    // LOG(WARNING) << "This is a warning message";
    // f.Encode({});
    

}