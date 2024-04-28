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

/*
-------------
|fec_index  |  
---------------------------            为了节省fec开销，每次编解码时，不以最大长度dataLen作为params.BlockBytes, 
|payload_len|   uint_16    ^           而是包组里面最大的 payload_len + sizeof(uint_16)
-------------             encode 
|           |             region       为了支持变长编解码，需要将payload_len也放在恢复区域，这样数据恢复出来之后可以
|  payload  |                          知道有效数据的真实长度是怎样的
|           |             v
---------------------------

*/

class Datagram{
    // uint16_t len;
private:
    uint8_t fec_index;
    uint8_t data[dataLen]; // ** 前两个byte存放长度  2^16 =65536 ** , 网络传输时需要htons
public:
    Datagram () {}
    // Datagram(const uint8_t* payload, uint16_t len) {
    //     SetPayloadLen(len);
    //     SetPayload(payload, len);
    // }
    template<typename  T>
    Datagram(const T* payload, uint16_t len) {
        DCHECK_GE(dataLen - sizeof(uint16_t), len) << "pay load too large, should be less than dataLen - sizeof(uint16_t)";
        SetPayloadLen(len);
        SetPayload(payload, len);
    }

    inline uint8_t GetFecIndex() const {
        return fec_index;
    }
    inline void SetFecIndex(uint8_t fec_index) {
        this->fec_index = fec_index;
    }

    inline uint16_t GetPayloadLen() const {
        return *(uint16_t*)(data);
    }
    inline void SetPayloadLen(uint16_t len) {
        *(uint16_t*)(data) = len;
    }
    inline void SetPayload(const void* payload, uint16_t len) {
        DCHECK_GE(dataLen - sizeof(uint16_t) /* exclude payload_len */, len) << "pay load too large, should be less than dataLen - sizeof(uint16_t)";
        memcpy(GetPayload(), payload, len);
    }
    //返回的是携带的真实数据, payload
    inline const uint8_t* GetPayload() const {
        return (uint8_t*)data + sizeof(uint16_t);

    }
    inline uint8_t* GetPayload() {
        return (uint8_t*)data + sizeof(uint16_t);
    }


    inline uint16_t GetCodecLen() const {
        return GetPayloadLen() + sizeof(uint16_t); // use 2 bytes to indicated payload length
    }
    
    inline void SetCodecData(const void* codec_data, uint16_t len) {
        DCHECK_GE(dataLen, len) << "codec data too large, should be less than dataLen";
        memcpy(GetCodecData(), codec_data, len);
    }

    // 返回的是需要fec处理的区域, payload_len + payload
    inline const uint8_t* GetCodecData() const {
        return (uint8_t*)data;

    }
    inline uint8_t* GetCodecData() {
        return (uint8_t*)data;
    }
    static Datagram* HostToNetwork(Datagram* d) {
        d->SetPayloadLen(htons(d->GetPayloadLen()));
        return d;
    } 
    static Datagram* NetworkToHost(Datagram* d) {
        d->SetPayloadLen(ntohs(d->GetPayloadLen()));
        return d;
    } 

};

void PrintDatagram(Datagram* d) {
    cout << "fec_index: " << static_cast<int>(d->GetFecIndex()) << endl;
    cout << "len: " << d->GetPayloadLen() << endl;
    cout << "content: ";
    for (size_t i = 0; i < d->GetPayloadLen(); ++i) {
        cout << ((char*)d->GetPayload())[i];
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
        params.BlockBytes = buffers.front()->GetCodecLen();
        for (int i = 0; i < buffers.size(); ++i) {
            // buffers[i].fec_index = cm256_get_original_block_index(params, i);
            blocks[i].Block = buffers.at(i)->GetCodecData();
            blocks[i].Index = cm256_get_original_block_index(params, i);
            buffers[i]->SetFecIndex(blocks[i].Index);
            params.BlockBytes = max(params.BlockBytes, (int)buffers.at(i)->GetCodecLen());
        }

        uint8_t* recovery_data = new uint8_t[info.redundancy_cnt * params.BlockBytes];
        cm256_encode(params, blocks, recovery_data);

        LOG(ERROR) << info.redundancy_cnt;
        for (int i = 0; i < info.redundancy_cnt; ++i) {
            auto ptr_d = new Datagram;


            ptr_d->SetFecIndex(cm256_get_recovery_block_index(params, i));
            ptr_d->SetCodecData(recovery_data + i * params.BlockBytes, params.BlockBytes);
            buffers.push_back(ptr_d);
        }


        delete[] recovery_data;
        return;
    }
    bool Decode(std::vector<Datagram*>& buffers, Fec_type type) {
        
        Fec_info info = GetInfoAboutFEC(type);
        if (buffers.size() < info.data_cnt) {
            return false;
        }
        // DCHECK_GE(buffers.size(), info.data_cnt) << "buffer size is not greater or equal to fec_type.data_cnt";
        params.BlockBytes = dataLen;
        for (int i = 0; i < info.data_cnt; ++i) {
            blocks[i].Block = buffers.at(i)->GetCodecData();
            blocks[i].Index = buffers.at(i)->GetFecIndex();
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
    // LOG(ERROR) << buffer.size();
    f.Encode(buffer, Fec_type::FEC_2_1);

    vector<Datagram*> new_buffer{buffer[0], buffer[2]};

    LOG(ERROR) << f.Decode(new_buffer, Fec_type::FEC_2_1);

    for (int i = 0; i < new_buffer.size(); ++i) {
        // new_buffer[i]->SetLen(buffer[i]->len;
        PrintDatagram(new_buffer.at(i));
    }
    

}