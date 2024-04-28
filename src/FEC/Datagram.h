#pragma once

#include <stdint.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <glog/logging.h>

constexpr int dataLen = 998;
class Datagram {
private:
    uint16_t len;
    uint8_t payload[dataLen];
public:
    inline uint16_t GetTotalLen() const {
        return offsetof(Datagram, payload) + len;
    }
    inline uint16_t GetPayloadLen() const {
        return len;
    }
    inline void SetPayloadLen(uint16_t len) {
        this->len = len;
    }
    inline void SetPayload(const void* payload, uint16_t len) {
        DCHECK_GE(dataLen, len) << "payload is too large to carry， len should be less than dataLen";
        memcpy(this->payload, payload, len);
    }
    inline uint8_t* GetPayload() const {
        return (uint8_t*)payload;
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
};
