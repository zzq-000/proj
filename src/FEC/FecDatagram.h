#pragma once
#include <string>

#include <glog/logging.h>
#include "Datagram.h"
enum class FecType{
    Fec_none = 0,
    FEC_2_1 = 1,
    FEC_3_1 = 2,
    FEC_3_2 = 3,
    FEC_4_1 = 4,
    FEC_4_2 = 5,
    FEC_4_3 = 6,
    FEC_5_1 = 7,
    FEC_5_2 = 8,
    FEC_5_3 = 9,
    FEC_5_4 = 10,
    FEC_6_1 = 11,
    FEC_6_2 = 12,
    FEC_6_3 = 13,
    FEC_6_4 = 14,
    FEC_7_1 = 15,
};

struct FecInfo{
    int data_cnt;
    int redundancy_cnt;
    std::string type;
    inline int TotalCount() const {
        return data_cnt + redundancy_cnt;
    }
};



FecInfo GetInfoAboutFEC(FecType t) {
    switch (t)
    {
    case FecType::Fec_none:
        return {0, 0, "FEC_none"};
    case FecType::FEC_2_1:
        return {2, 1, "FEC_2_1"};
    case FecType::FEC_3_1:
        return {3, 1, "FEC_3_1"};
    case FecType::FEC_3_2:
        return {3, 2, "FEC_3_2"};
    case FecType::FEC_4_1:
        return {4, 1, "FEC_4_1"};
    case FecType::FEC_4_2:
        return {4, 2, "FEC_4_2"};
    case FecType::FEC_4_3:
        return {4, 3, "FEC_4_3"};
    case FecType::FEC_5_1:
        return {5, 1, "FEC_5_1"};
    case FecType::FEC_5_2:
        return {5, 2, "FEC_5_2"};
    case FecType::FEC_5_3:
        return {5, 3, "FEC_5_3"};
    case FecType::FEC_5_4:
        return {5, 4, "FEC_5_4"};
    case FecType::FEC_6_1:
        return {6, 1, "FEC_6_1"};
    case FecType::FEC_6_2:
        return {6, 2, "FEC_6_2"};
    case FecType::FEC_6_3:
        return {6, 3, "FEC_6_3"};
    case FecType::FEC_6_4:
        return {6, 4, "FEC_6_4"};
    case FecType::FEC_7_1:
        return {7, 1, "FEC_7_1"};
    default:
        LOG(ERROR) << "no such fec_type";
        return {};
    }
}



class FecDatagram {

public:
    virtual inline uint8_t GetFecIndex() const = 0;
    virtual inline void SetFecIndex(uint8_t fec_index) = 0;

    virtual inline uint16_t GetPacketPayloadLen() const = 0;
    virtual inline void SetPacketPayloadLen(uint16_t len) = 0;

    virtual inline void SetPacketPayload(const void* payload, uint16_t len) = 0;
    //返回的是携带的真实数据, payload
    virtual inline const uint8_t* GetPacketPayload() const = 0;
    virtual inline uint8_t* GetPacketPayload() = 0;


    virtual inline uint16_t GetPacketLen() const = 0;
    virtual inline void SetPacket(Datagram* p) = 0;
    virtual inline void SetPacket(const void* codec_data, uint16_t len) = 0;

    // 返回的是需要fec处理的区域, payload_len + payload
    virtual inline const uint8_t* GetPacket() const = 0;
    virtual inline uint8_t* GetPacket() = 0;
};

class BlockFecDatagram: public FecDatagram{
    // uint16_t len;
private:
    uint8_t fec_index;
    Datagram* packet; 
public:
    BlockFecDatagram (): fec_index(0) {
        packet = new Datagram;
    }
    BlockFecDatagram(const BlockFecDatagram& rhs):fec_index(rhs.fec_index) {
        // fec_index = rhs.fec_index;
        packet = new Datagram;
        memcpy(packet, rhs.packet, sizeof(Datagram));
        // packet = rhs.packet;
    };
    BlockFecDatagram(Datagram&&) = delete;

    template<typename  T>
    BlockFecDatagram(const T* payload, uint16_t len):fec_index(0) {
        packet = new Datagram;
        SetPacketPayloadLen(len);
        SetPacketPayload(payload, len);
    }

    inline uint8_t GetFecIndex() const override{
        return fec_index;
    }
    inline void SetFecIndex(uint8_t fec_index) override{
        this->fec_index = fec_index;
    }

    inline uint16_t GetPacketPayloadLen() const override{
        return packet->GetPayloadLen();
    }
    inline void SetPacketPayloadLen(uint16_t len) override{
        packet->SetPayloadLen(len);
    }

    inline void SetPacketPayload(const void* payload, uint16_t len) override{
        packet->SetPayload(payload, len);
    }
    //返回的是携带的真实数据, payload
    inline const uint8_t* GetPacketPayload() const override{
        return packet->GetPayload();
    }
    inline uint8_t* GetPacketPayload() override{
        return packet->GetPayload();
    }


    inline uint16_t GetPacketLen() const override{
        return packet->GetTotalLen();
    }
    
    inline void SetPacket(const void* codec_data, uint16_t len) override{
        DCHECK_GE(sizeof(Datagram), len) << "codec data too large, should be less than dataLen";
        memcpy(GetPacket(), codec_data, len);
    }
    inline void SetPacket(Datagram* p) {
        packet = p;
    }
    // 返回的是需要fec处理的区域, payload_len + payload
    inline const uint8_t* GetPacket() const override{
        return (uint8_t*)packet;
    }
    inline uint8_t* GetPacket() override{
        return (uint8_t*)packet;
    }
    bool operator==(const BlockFecDatagram& rhs) const {
        return *packet == *rhs.packet;
    }
    ~BlockFecDatagram() {
        if (packet) {
            delete packet;
            packet = NULL;
        }
    }
    static void RandomBlockFecDatagram(BlockFecDatagram* fec_datagram) {
        Datagram::RandomDatagram(*(fec_datagram->packet));
        // fec_datagram->SetPacket(&d, d.GetTotalLen());
    }

    static BlockFecDatagram* HostToNetwork(BlockFecDatagram* d) {
        Datagram::HostToNetwork(d->packet);
        return d;
    } 
    static BlockFecDatagram* NetworkToHost(BlockFecDatagram* d) {
        Datagram::NetworkToHost(d->packet);
        return d;
    } 
    static uint16_t GetMaxPacketLen() {
        return Datagram::GetMaxLen();
    }

};
std::ostream& operator<<(std::ostream& os, const BlockFecDatagram& datagram) {
    os << "BlockFecDatagram, len: " << datagram.GetPacketPayloadLen() ;
    return os;
}