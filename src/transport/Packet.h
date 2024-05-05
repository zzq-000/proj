#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>
#include "glog/logging.h"
#include "FecDatagram.h"


#define GET_ALIGNED_SIZE(size, align) (((size) / (align) + ((size) % (align) > 0)) * (align))
#define GET_ALIGNED_SIZE_DELETE_EXTRA(size, align) (((size) / (align)) * (align))

static constexpr int kHeaderBitLen = 16;
constexpr int MTU = 1500;
constexpr int IPHeader = 20;
constexpr int UDPHeader = 8;
constexpr int FECMaxDataLen = MTU - IPHeader - UDPHeader
                                 - sizeof(uint8_t)/*FecDatagram::fec_index*/ - sizeof(uint16_t)/*Datagram::len*/;


size_t GetAlignedSize(int size, int align) {
    return (size / align + (size % align > 0)) * align;
}

// class FeedbackPacket{


// };
void printBinary(uint8_t num) {
    for (int i = 7; i >= 0; i--) {
        std::cout << ((num >> i) & 1);  // 右移并与1进行AND操作以获取当前位
    }
    std::cout << std::endl;
}

enum class Packet_type{
    DataPacket = 1,
    FeedbackPacket = 2,
    ProbeRequestPacket = 3,
    ProbeResponsePacket = 4,
    SessionRequestPacket = 5,
    SessionResponsePacket = 6,
    SessionStartPacket = 7
};

class Packet{
private:
    uint64_t seq_num;
    uint16_t len;
    
    // 6 * 8 = 48 bit
    // 第48 bit表示eof, 发送方表示传输结束
    // 第47bit表示start, 接收方向服务器发送请求, 此时表示接收方希望以seq_num开始发送
    // 43-46bit 表示fec_type
    // 41 42 todo
    // 37-40bit 表示包的类型     
    //      0001 dataPacket
    //      0010 feedbackPacket
    //      0011 probeRequestPacket
    //      0100 probeResponsePacket
    //      0101 FileRequestPacket
    //      0110 
    //                     Packet_type       fec_type start eof
    // [1 1 1 1 1 ....      1 1 1 1      00   1 1 1 1  1     1]   len = 48
    uint8_t header[6];
    union {
        uint8_t data[GET_ALIGNED_SIZE_DELETE_EXTRA(FECMaxDataLen - GET_ALIGNED_SIZE(sizeof(seq_num) + sizeof(len) + sizeof(header), sizeof(uint64_t)), sizeof(uint64_t))];
    };


public:
    Packet() = default;

    template<typename T>
    Packet(T* content, int length){
        SetContent(content, length);
    }

    template<typename T>
    void SetContent(T* content, int length) {
        DCHECK_GE(sizeof(data), length) << "the length of the content is too large";
        len = length;
        memcpy(data, content, len);
    }

    uint16_t GetContentLen() const {
        return len;
    }
    void SetContentLen(uint16_t length) {
        len = length;
    }
    uint8_t* GetContent() {
        return data;
    }
    const uint8_t* GetContent() const {
        return data;
    }

    uint64_t GetSequenceNum() const {
        return seq_num;
    }
    void SetSequenceNum(uint64_t seq) {
        seq_num = seq;
    }
    bool IsStart() const {
        // printBinary(header[5]);
        // printBinary(header[5] & 0b10);
        return (header[5] & 0b10) != 0;
    }
    void SetStart() {
        // printBinary(header[5]);
        header[5] |= 0b10;
        // printBinary(header[5]);

    }
    bool IsEOF() const {
        return (header[5] & 1) == 1;
    };
    void SetEOF() {
        header[5] |= 1;
    }
    Packet_type GetPacketType() const {
          return static_cast<Packet_type>(header[4] & 0b1111);
    }
    void SetPacketType(Packet_type type) {
        header[4] = ((header[4] & 0b0000) | static_cast<uint8_t>(type));
    }
    int PacketSize() const {
        Packet_type type = GetPacketType();
        switch (type)
        {
        case Packet_type::DataPacket:
            return GetHeaderSize() + len;
        default:
            return GetHeaderSize();
        }
    }
    void Clear() {
        memset(data, 0, sizeof(data));
        memset(header, 0, sizeof(header));
        len = 0;
        seq_num = 0;
    }
    static uint8_t GetHeaderSize() {
        return sizeof(seq_num) + sizeof(len) + sizeof(header);
    }
    static Packet GenerateRandomPacket() {
        Packet p;
        p.seq_num = rand() % 1000 + 100;
        p.len = rand() % sizeof(data);
        for (int i = 0; i < p.len; ++i) {
            p.data[i] = rand() % 10000 + rand() % 9999;
        }
        p.SetPacketType(Packet_type::DataPacket);
        return p;
    }
    static Packet ParseFromData(const void* content, int len) {
        Packet p;
        
        uint8_t* data = (uint8_t*)content;
        memcpy(&p, data, Packet::GetHeaderSize());
        // LOG(INFO) << static_cast<int>(Packet::GetHeaderSize());
        // LOG(INFO) << len;
        // LOG(INFO) << len - Packet::GetHeaderSize();
        // LOG(INFO) << p.GetContentLen();

        if (len - Packet::GetHeaderSize() != p.GetContentLen()) {
            LOG(FATAL) << "invalid packet";
        }
        // DCHECK_EQ((int)p.GetContentLen(), len - Packet::GetHeaderSize()) << "invalid packet";
        if (p.PacketSize() > Packet::GetHeaderSize()) {
            memcpy(p.data, data + Packet::GetHeaderSize(), p.GetContentLen());
        }
        return p;
    }

    template<typename T>
    static std::vector<Packet> GeneratePackets(T* buffer, int len, int start_seq = 0) {
        int size = (len / sizeof(data) + (len % sizeof(data) > 0));
        std::vector<Packet> rtn(size);
        uint8_t* ptr = (uint8_t*)buffer;
        for (int i = 0; i < size; ++i) {
            if (i != size - 1) {
                rtn[i].SetContent(ptr, sizeof(data));
                ptr += sizeof(data);
            }else {
                rtn[i].SetContent(ptr, len % sizeof(data));
                rtn[i].SetEOF();
            }
            rtn[i].SetSequenceNum(i + start_seq);
        }
        rtn[0].SetStart();
        return rtn;
    } 
};

