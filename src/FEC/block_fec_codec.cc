#include "fec_codec.h"
#include "fec_util.h"

// the length of each element in buffers should be ge than encode_length
bool BlockFecCodec::Encode(std::vector<void*>& buffers, FecType type, int encode_length) {
    FecInfo info = GetInfoAboutFEC(type);
    DCHECK_EQ(buffers.size(), info.data_cnt + info.redundancy_cnt) << "buffers' size should be equal to info.data_cnt + info.redundancy_cnt";

    params.OriginalCount = info.data_cnt;
    params.RecoveryCount = info.redundancy_cnt;
    params.BlockBytes = encode_length;
    for (int i = 0; i < info.data_cnt; ++i) { 
        blocks[i].Block = buffers[i];
    }

    for (int i = info.data_cnt; i < info.data_cnt + info.redundancy_cnt; ++i) {
        cm256_encode_block(params, blocks, i, buffers[i]);
    }
    return true;
}

bool BlockFecCodec::Decode(std::vector<void*>& buffers, FecType type, int decode_length) { 
    FecInfo info = GetInfoAboutFEC(type);
    DCHECK_EQ(buffers.size(), info.data_cnt + info.redundancy_cnt) << "buffers' size should be equal to info.data_cnt + info.redundancy_cnt";
    if (info.redundancy_cnt == 0) {
        return true;
    }
    params.OriginalCount = info.data_cnt;
    params.RecoveryCount = info.redundancy_cnt;
    params.BlockBytes = decode_length;

    int redundant_index = info.data_cnt;
    for (int i = 0; i < info.data_cnt; ++i) {
        if (buffers[i] != NULL) {
            blocks[i].Block = buffers[i];
            blocks[i].Index = i;
        } else {
            while (buffers[redundant_index] == NULL) redundant_index++;
            blocks[i].Block = buffers[redundant_index];
            blocks[i].Index = redundant_index;
            redundant_index++;
        }
    }
    bool ans = (cm256_decode(params, blocks) == 0);

    for (int i = 0; i < info.data_cnt; ++i) {
        if (buffers[i] == NULL) {
            buffers[i] = blocks[i].Block;
        }
    }
    return ans;
}