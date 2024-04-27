#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
const int dataSize = 10;
struct Packet
{
    uint32_t len;
    uint8_t data[dataSize];
};
