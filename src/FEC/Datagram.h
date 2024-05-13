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
    uint8_t* buffer;

public:
    Datagram():len(0), buffer(NULL) {

    }


    Datagram(void* ptr, uint16_t len): len(len), buffer((uint8_t*)ptr) {

    }

};