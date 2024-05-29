#include "receiver_worker.h"
#include "util/timestamp.h"

RWorker::RWorker(Config config): 
    config_(config), cache_(), codec_(), last_submit_seq_(0), last_submit_time_(0), received_first_(false) {}


//尽力而为按序交付

// TODO, 如何触发nack
std::list<DataPacket*> RWorker::GetApplicationMessages(const Packet& packet) {
    std::list<DataPacket*> rtn;
    cache_.CachePacket(packet);
    FecType type = packet.fec_type();
    FecInfo info = GetInfoAboutFEC(type);

    if (type == FecType::FEC_NONE) {
        Packet* p = cache_.FindPacket(packet.seq_num());
        if (p != NULL) {
            rtn.push_back(const_cast<DataPacket*>(&(p->data_packet())));
        }
        received_first_ = true;
        last_submit_seq_ = p->seq_num();
        last_submit_time_ = timestamp_ms();

    } else {
        uint64_t start_seq = packet.seq_num() - packet.fec_index();

        // 该组信息已被提交
        if (start_seq + info.TotalCount() <= last_submit_seq_) {
            return rtn;
        }

        // 乱序收到后一个包组的包， 不用处理
        if (start_seq != last_submit_seq_ + 1) {
            return rtn;
        }
        
        assert(start_seq == last_submit_seq_ + 1);
        // 不可能， 如果已被提交， 则改组信息不管是否能够恢复， 后续都不管了， 直接将last_submit_seq_后置
        // if (start_seq <= last_submit_seq_)  

        bool data_loss = false;
        int loss_num = 0;
        for (uint64_t i = start_seq; i < start_seq + info.TotalCount(); ++i) {
            if (cache_.FindPacket(i) == NULL) {
                if (i < start_seq + info.data_cnt) {
                    data_loss = true;
                }
                loss_num ++;
            }
        }


        if (!data_loss) {
            for (uint64_t i = start_seq; i < start_seq + info.data_cnt; ++i) {
                Packet* p = cache_.FindPacket(i);
                rtn.push_back(const_cast<DataPacket*>(&(p->data_packet())));
            }
            last_submit_seq_ = start_seq + info.TotalCount() - 1;
            last_submit_time_ = timestamp_ms();
        } else if (loss_num <= info.redundancy_cnt) {
            int decode_size = 0;
            for (uint64_t i = start_seq + info.data_cnt; i < start_seq + info.TotalCount(); ++i) {
                Packet* p = cache_.FindPacket(i);
                if (p) {
                    decode_size = p->subpacket_len();
                    break;
                }
            }
            assert(decode_size != 0);
            uint8_t* buffer = new uint8_t[decode_size * info.TotalCount()];
            std::vector<void*> data(info.TotalCount());
            for (uint64_t i = start_seq; i < start_seq + info.TotalCount(); ++i) {
                Packet* p = cache_.FindPacket(i);
                if (p) {
                    bool ret = p->data_packet().SerializeToArray(buffer + i * decode_size, p->data_packet().ByteSizeLong());
                    DCHECK_EQ(ret, true) << "failed to serialize data packet to array";
                    data[i] = buffer + i * decode_size;
                }else {
                    data[i] = NULL;
                }
                // p->FromA
            }
            bool decode_ans = codec_.Decode(data, type, decode_size);
            DCHECK_EQ(decode_ans, true) << "failed to decode data";


            //   可以恢复
        }else {
            // 1. 真的丢失了， 无法恢复
            // 只有当start_seq 刚好等于last_submit_seq_时， 才考虑超时的问题
            // 如果是后面一个fec包组里面的数据包来了， 不管是否超时， 继续等着
            if (start_seq == last_submit_seq_ + 1 &&
                 timestamp_ms() - last_submit_time_ >= config_.fec_decode_wait_time_limit_ms) {
                for (uint64_t i = start_seq; i < start_seq + info.data_cnt; ++i) {
                    Packet* p = cache_.FindPacket(i);
                    if (p) {
                        rtn.push_back(const_cast<DataPacket*>(&(p->data_packet())));
                    }
                }
                last_submit_seq_ = start_seq + info.TotalCount() - 1;
                last_submit_time_ = timestamp_ms();
            } else {
                // 2. 有可能只是剩下的未接收到, 不用管， 等待接收后续的包即可
            }
            
        }
    }
    return rtn;

    
}