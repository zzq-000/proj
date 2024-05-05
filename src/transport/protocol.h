
#include "Packet.h"

enum class Return_Code{
    SUCCESS = 0,
    FAIL = 1
};



class Protocol{
public:
    // todo SetStart是否有必要？
    static Packet RegisterSessionRequestPacket(uint64_t start_seq) {
        Packet p;
        p.SetSequenceNum(start_seq);
        p.SetStart();
        p.SetPacketType(Packet_type::SessionRequestPacket);
        p.SetContentLen(0);
        return p;
    }
    static bool CheckSessionRequestPacket(const Packet& p) {
        return (p.IsStart() && p.GetPacketType() == Packet_type::SessionRequestPacket);
    }
    static Return_Code RegisterSessionResponsePacket(const Packet& in_packet, Packet& out_packet) {
        if (CheckSessionRequestPacket(in_packet)) {
            out_packet.Clear();
            out_packet.SetSequenceNum(in_packet.GetSequenceNum() + 1);
            out_packet.SetPacketType(Packet_type::SessionResponsePacket);
            out_packet.SetContentLen(0);
            return Return_Code::SUCCESS;
        }
        return Return_Code::FAIL;
    }
    static bool CheckSessionResponsePacket(const Packet& p) {
        return p.GetPacketType() == Packet_type::SessionResponsePacket;
    }
    static bool CheckSessionPacketsMatch(const Packet& req_packet, const Packet& resp_packet) {
        return CheckSessionRequestPacket(req_packet) && CheckSessionResponsePacket(resp_packet)
            && resp_packet.GetSequenceNum() == req_packet.GetSequenceNum() + 1;
    }
};