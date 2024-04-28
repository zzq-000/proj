
#include "gtest/gtest.h"
#include "Datagram.h"
#include "FecDatagram.h"
#include "FecCodec.h"
using namespace std;
using namespace google;



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





void PrintFecDatagram(FecDatagram* d) {
    cout << "fec_index: " << static_cast<int>(d->GetFecIndex()) << endl;
    cout << "len: " << d->GetPacketPayloadLen() << endl;
    cout << "content: ";
    for (size_t i = 0; i < d->GetPacketPayloadLen(); ++i) {
        cout << ((char*)d->GetPacketPayload())[i];
    }
    cout << endl << endl;;
}


TEST(BlockFecCodec, basic) {
    BlockFecCodec f;

    const char* s1 = "hello, world";
    BlockFecDatagram d1(s1, strlen(s1));


    // memcpy(d1.data, s1, strlen(s1));
    // d1.SetLen(strlen(s1));

    const char* s2 = "thank you";
    BlockFecDatagram d2(s2, strlen(s2));
    // memcpy(d2.data, s2, strlen(s2));
    // d2.len = strlen(s2);

    vector<FecDatagram*> buffer;
    buffer.emplace_back(&d1);
    buffer.emplace_back(&d2);
    // LOG(ERROR) << buffer.size();
    f.Encode(buffer, Fec_type::FEC_2_1);

    vector<FecDatagram*> new_buffer{buffer[0], buffer[2]};
    google::InitGoogleLogging("test");
    LOG(ERROR) << f.Decode(new_buffer, Fec_type::FEC_2_1);

    for (int i = 0; i < new_buffer.size(); ++i) {
        // new_buffer[i]->SetLen(buffer[i]->len;
        PrintFecDatagram(new_buffer.at(i));
    }
}
// int main(int argc, char** argv) {
    // google::InitGoogleLogging(argv[0]);
//     google::SetLogDestination(google::INFO, "./");
//     google::SetLogFilenameExtension("log_");
//     google::InstallFailureSignalHandler();

//     BlockFecCodec f;

//     const char* s1 = "hello, world";
//     BlockFecDatagram d1(s1, strlen(s1));


//     // memcpy(d1.data, s1, strlen(s1));
//     // d1.SetLen(strlen(s1));

//     const char* s2 = "thank you";
//     BlockFecDatagram d2(s2, strlen(s2));
//     // memcpy(d2.data, s2, strlen(s2));
//     // d2.len = strlen(s2);

//     vector<FecDatagram*> buffer;
//     buffer.emplace_back(&d1);
//     buffer.emplace_back(&d2);
//     // LOG(ERROR) << buffer.size();
//     f.Encode(buffer, Fec_type::FEC_2_1);

//     vector<FecDatagram*> new_buffer{buffer[0], buffer[2]};

//     LOG(ERROR) << f.Decode(new_buffer, Fec_type::FEC_2_1);

//     for (int i = 0; i < new_buffer.size(); ++i) {
//         // new_buffer[i]->SetLen(buffer[i]->len;
//         PrintFecDatagram(new_buffer.at(i));
//     }
    

// }