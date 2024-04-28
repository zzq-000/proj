#include "FecCodec.h"


void BlockFecCodec::Encode(std::vector<FecDatagram*>& buffers, Fec_type type) {
    if (buffers.empty()) return;
        Fec_info info = GetInfoAboutFEC(type);
        DCHECK_EQ(buffers.size(), info.data_cnt) << "buffer size is not equal to fec_type.data_cnt";
        params.OriginalCount = buffers.size();
        params.RecoveryCount = info.redundancy_cnt;
        params.BlockBytes = buffers.front()->GetPacketLen();
        for (int i = 0; i < buffers.size(); ++i) { 
            blocks[i].Block = buffers.at(i)->GetPacket();
            blocks[i].Index = cm256_get_original_block_index(params, i);
            buffers[i]->SetFecIndex(blocks[i].Index);
            params.BlockBytes = std::max(params.BlockBytes, (int)buffers.at(i)->GetPacketLen());
        }

        uint8_t* recovery_data = new uint8_t[info.redundancy_cnt * params.BlockBytes];
        cm256_encode(params, blocks, recovery_data);


        for (int i = 0; i < info.redundancy_cnt; ++i) {
            auto ptr_d = new BlockFecDatagram;
            ptr_d->SetFecIndex(cm256_get_recovery_block_index(params, i));
            ptr_d->SetPacket(recovery_data + i * params.BlockBytes, params.BlockBytes);
            buffers.push_back(ptr_d);
        }
        delete[] recovery_data;
        return;
}

bool BlockFecCodec::Decode(std::vector<FecDatagram*>& buffers, Fec_type type) { 
    Fec_info info = GetInfoAboutFEC(type);
    if (buffers.size() < info.data_cnt) {
        return false;
    }
    params.BlockBytes = BlockFecDatagram::GetMaxPacketLen();
    for (int i = 0; i < info.data_cnt; ++i) {
        blocks[i].Block = buffers.at(i)->GetPacket();
        blocks[i].Index = buffers.at(i)->GetFecIndex();
    } 
    return cm256_decode(params, blocks);
}