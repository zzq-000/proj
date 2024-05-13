#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>
#include "glog/logging.h"
#include "FecDatagram.h"


#define GET_ALIGNED_SIZE(size, align) (((size) / (align) + ((size) % (align) > 0)) * (align))
#define GET_ALIGNED_SIZE_DELETE_EXTRA(size, align) (((size) / (align)) * (align))

static constexpr int kHeaderBitLen = 16;
constexpr int kMTU = 1500;
constexpr int kIPHeader = 20;
constexpr int kUDPHeader = 8;
constexpr int kDataLen = kMTU - kIPHeader - kUDPHeader;
constexpr int kNackNum = 120;


size_t GetAlignedSize(int size, int align) {
    return (size / align + (size % align > 0)) * align;
}


enum class PacketType{
    DataPacket = 1,
    FeedbackPacket = 2,
    ProbeRequestPacket = 3,
    ProbeResponsePacket = 4,
    SessionRequestPacket = 5,
    SessionResponsePacket = 6,
    SessionStartPacket = 7
};

class DataPacket{
public:
    uint16_t len;
    uint8_t data[kDataLen];
};
class FeedbackPacket{
    double lossrate;
    uint16_t rtt;
    uint64_t nack[kNackNum];
};
class ProbePacket{
    bool request; // true for request, false for response
};

class Packet{
private:
    uint64_t seq_num = 0;
    uint8_t packet_type = 0;
    uint8_t fec_type = 0;
    uint8_t fec_index = 0; // 当前packet在对应的fec包组中的位置
    
    union{
        DataPacket data_packet;
        FeedbackPacket feedback_packet;
        ProbePacket probe_packet;
    };

public:
    Packet() = default;

    inline uint64_t GetSequenceNum() const {
        return seq_num;
    }
    inline uint16_t GetPacketSize() const {
        if (GetPacketType() == PacketType::DataPacket) {
            return offsetof(Packet, data_packet) + data_packet.len;
        }
    }

    inline void ClearDataPacket() {
        DCHECK_EQ(GetPacketType(), PacketType::DataPacket) << "only data packet can clear self";
        data_packet.len = 0;
    }

    inline void SetSequenceNum(uint64_t seq) {
        seq_num = seq;
    }
    inline void SetFecType(FecType type) {
        fec_type = static_cast<int>(type);
    }
    inline void SetPacketType(PacketType type) {
        packet_type = static_cast<int>(type);
    }
    inline PacketType GetPacketType() const {
        return static_cast<PacketType>(packet_type);
    }
    inline void SetFecIndex(uint8_t fec_index) {
        this->fec_index = fec_index;
        
    }
};

