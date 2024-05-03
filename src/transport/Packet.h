#include <stdint.h>
#include <vector>
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

class FeedbackPacket{


};

class Packet{
private:
    uint64_t seq_num;
    uint16_t len;
    
    // 最后1bit表示eof

    // 倒数第2bit表示start      
    //                  proberesp      probereq    feedback  fec_type strat eof
    // [1 1 1 1 1 ....      1              1         1        1 1 1 1  1     1]   len = 48
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
        return (header[5] & 2) == 1;
    }
    void SetStart() {
        header[5] |= 2;
    }
    bool IsEOF() const {
        return (header[5] & 1) == 1;
    };
    void SetEOF() {
        header[5] |= 1;
    }
    int PacketSize() const {
        return GetHeaderSize() + len;
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

