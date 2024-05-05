#pragma once
#include <string>

#include <glog/logging.h>
#include "Datagram.h"
enum class Fec_type{
    FEC_2_1 = 1,
    FEC_3_1 = 2,
    FEC_3_2 = 3,
    FEC_4_1 = 4,
    FEC_4_2 = 5,
    FEC_4_3 = 6,
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
    case Fec_type::FEC_4_1 :
        return {4, 1, "FEC_4_1"};
    case Fec_type::FEC_4_2 :
        return {4, 2, "FEC_4_2"};
    case Fec_type::FEC_4_3 :
        return {4, 3, "FEC_4_3"};
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
    
    virtual inline void SetPacket(const void* codec_data, uint16_t len) = 0;

    // 返回的是需要fec处理的区域, payload_len + payload
    virtual inline const uint8_t* GetPacket() const = 0;
    virtual inline uint8_t* GetPacket() = 0;
};

class BlockFecDatagram: public FecDatagram{
    // uint16_t len;
private:
    uint8_t fec_index;
    Datagram packet; 
public:
    BlockFecDatagram () {}

    BlockFecDatagram(Datagram&) = delete;
    BlockFecDatagram(Datagram&&) = delete;

    template<typename  T>
    BlockFecDatagram(const T* payload, uint16_t len) {
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
        return packet.GetPayloadLen();
    }
    inline void SetPacketPayloadLen(uint16_t len) override{
        packet.SetPayloadLen(len);
    }

    inline void SetPacketPayload(const void* payload, uint16_t len) override{
        packet.SetPayload(payload, len);
    }
    //返回的是携带的真实数据, payload
    inline const uint8_t* GetPacketPayload() const override{
        return packet.GetPayload();
    }
    inline uint8_t* GetPacketPayload() override{
        return packet.GetPayload();
    }


    inline uint16_t GetPacketLen() const override{
        return packet.GetTotalLen();
    }
    
    inline void SetPacket(const void* codec_data, uint16_t len) override{
        DCHECK_GE(sizeof(Datagram), len) << "codec data too large, should be less than dataLen";
        memcpy(GetPacket(), codec_data, len);
    }

    // 返回的是需要fec处理的区域, payload_len + payload
    inline const uint8_t* GetPacket() const override{
        return (uint8_t*)&packet;
    }
    inline uint8_t* GetPacket() override{
        return (uint8_t*)&packet;
    }

    static void RandomBlockFecDatagram(BlockFecDatagram* fec_datagram) {
        Datagram d = Datagram::RandomDatagram();
        fec_datagram->SetPacket(&d, d.GetTotalLen());
    }

    static BlockFecDatagram* HostToNetwork(BlockFecDatagram* d) {
        Datagram::HostToNetwork(&d->packet);
        return d;
    } 
    static BlockFecDatagram* NetworkToHost(BlockFecDatagram* d) {
        Datagram::NetworkToHost(&d->packet);
        return d;
    } 
    static uint16_t GetMaxPacketLen() {
        return Datagram::GetMaxLen();
    }

};