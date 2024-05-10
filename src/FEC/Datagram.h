#pragma once

#include <stdint.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <glog/logging.h>

constexpr int dataLen = 998;
class Datagram {
private:
    uint16_t len;
    uint8_t payload[dataLen];
public:
    Datagram() {}
    Datagram(const Datagram& rhs) {
        SetPayloadLen(rhs.GetPayloadLen());
        SetPayload(rhs.GetPayload(), rhs.GetPayloadLen());
    }

    inline uint16_t GetTotalLen() const {
        return offsetof(Datagram, payload) + len;
    }
    inline uint16_t GetPayloadLen() const {
        return len;
    }
    inline void SetPayloadLen(uint16_t length) {
        this->len = length;
    }
    inline void SetPayload(const void* payload, uint16_t len) {
        DCHECK_GE(dataLen, len) << "payload is too large to carry， len should be less than dataLen";
        memcpy(this->payload, payload, len);
    }
    inline uint8_t* GetPayload() const {
        return (uint8_t*)payload;
    }
    bool operator==(const Datagram& rhs) const {
        // LOG(INFO) <<
        return GetPayloadLen() == rhs.GetPayloadLen() 
            && memcmp(GetPayload(), rhs.GetPayload(), std::min(GetPayloadLen(), (uint16_t)dataLen)) == 0;
    }
    static void RandomDatagram(Datagram& d) {
        uint16_t len = rand() % dataLen;
        d.SetPayloadLen(len);
        uint8_t* data = new uint8_t[len];
        for (int i = 0; i < len; ++i) {
            data[i] = rand() % UINT8_MAX;
        }
        d.SetPayload(data, len);
        // LOG(INFO) << d.GetPayloadLen();
        
    }
    static void HostToNetwork(Datagram* d) {
        d->SetPayloadLen(htons(d->GetPayloadLen()));
    } 
    static void NetworkToHost(Datagram* d) {
        d->SetPayloadLen(ntohs(d->GetPayloadLen()));
    } 
    static uint16_t GetMaxLen() {
        return offsetof(Datagram, len) + dataLen;
    }
    void LoadFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(this), sizeof(Datagram));
            file.close();
        } else {
            std::cerr << "Failed to open file for reading." << std::endl;
        }
    }

    // 函数用于将对象的二进制表示写入文件
    void DumpToFile(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<char*>(this), sizeof(Datagram));
            file.close();
        } else {
            std::cerr << "Failed to open file for writing." << std::endl;
        }
    }
};
