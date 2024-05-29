#include "receiver_worker.h"
#include "util/timestamp.h"

RWorker::RWorker(Config config): 
    config_(config), cache_(), codec_(), 
    next_submit_seq_(0), last_submit_time_(0), received_first_(false) { }


//尽力而为按序交付

// TODO, 如何触发nack
void RWorker::GetApplicationMessages(const Packet& packet, std::list<DataPacket*>& rtn) {
    LOG(ERROR) << packet.fec_index();
    cache_.CachePacket(packet);
    FecType type = packet.fec_type();
    FecInfo info = GetInfoAboutFEC(type);

    if (type == FecType::FEC_NONE) {
        Packet* p = cache_.FindPacket(packet.seq_num());
        if (p != NULL) {
            rtn.push_back(const_cast<DataPacket*>(&(p->data_packet())));
        }
        received_first_ = true;
        next_submit_seq_ = p->seq_num() + 1;
        last_submit_time_ = timestamp_ms();

    } else {
        uint64_t start_seq = packet.seq_num() - packet.fec_index();
        if (!received_first_) {
            received_first_ = true;
            next_submit_seq_ = start_seq;
            last_submit_time_ = timestamp_ms();
        }


        // 该组信息已被提交
        if (start_seq + info.TotalCount() <= next_submit_seq_) {
            return;
        }

        // 乱序收到后一个包组的包， 不用处理
        if (start_seq != next_submit_seq_) {
            return;
        }
        
        assert(start_seq == next_submit_seq_);
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
            next_submit_seq_ = start_seq + info.TotalCount();
            last_submit_time_ = timestamp_ms();
        } else if (loss_num <= info.redundancy_cnt) {
            int decode_size = 0;
            for (uint64_t i = start_seq + info.data_cnt; i < start_seq + info.TotalCount(); ++i) {
                Packet* p = cache_.FindPacket(i);
                if (p) {
                    decode_size = p->data_packet().len();
                    break;
                }
            }
            assert(decode_size != 0);
            std::vector<void*> data(info.TotalCount());
            for (uint64_t i = start_seq; i < start_seq + info.TotalCount(); ++i) {
                Packet* p = cache_.FindPacket(i);
                if (p) {
                    if (i < start_seq + info.data_cnt) {
                        bool ret = p->data_packet().SerializeToArray(buffer_ + (i - start_seq) * decode_size, p->data_packet().ByteSizeLong());
                        DCHECK_EQ(ret, true) << "failed to serialize data packet to array";
                    }else {
                        // LOG(INFO) << p->data_packet().data().size();
                        memcpy(buffer_ + (i - start_seq) * decode_size, p->data_packet().data().data(), p->data_packet().data().size());
                    }
                    data[(i - start_seq)] = buffer_ + (i - start_seq) * decode_size;
                }else {
                    data[(i - start_seq)] = NULL;
                }
            }
            LOG(INFO) << "decode_size: " << decode_size;
            bool decode_ans = codec_.Decode(data, type, decode_size);
            DCHECK_EQ(decode_ans, true) << "failed to decode data";
            for (uint64_t i = start_seq; i < start_seq + info.data_cnt; ++i) {
                Packet* p = cache_.FindPacket(i);
                if (p) {
                    rtn.push_back(const_cast<DataPacket*>(&(p->data_packet())));
                }else {
                    std::unique_ptr<DataPacket> ptr = std::make_unique<DataPacket>();
                    // LOG(INFO) << i << " " << i - start_seq;
                    ptr->ParseFromArray(data[i - start_seq], decode_size);
                    // LOG(INFO) << ptr->len();
                    assert(ptr->data().size() >= ptr->len());

                    // ptr->set_len()
                    Packet* tmp_ptr = cache_.GetNextPlaceToCache(i);
                    tmp_ptr->set_allocated_data_packet(ptr.release());
                    tmp_ptr->set_fec_index(i - start_seq);
                    tmp_ptr->set_seq_num(i);
                    rtn.push_back(const_cast<DataPacket*>(&(tmp_ptr->data_packet())));
                }
            }
            next_submit_seq_ = start_seq + info.TotalCount();
            last_submit_time_ = timestamp_ms();
            // for ()
            //   可以恢复
        }else {
            // 1. 真的丢失了， 无法恢复
            // 只有当start_seq 刚好等于last_submit_seq_时， 才考虑超时的问题
            // 如果是后面一个fec包组里面的数据包来了， 不管是否超时， 继续等着
            if (start_seq == next_submit_seq_ + 1 &&
                 timestamp_ms() - last_submit_time_ >= config_.fec_decode_wait_time_limit_ms) {
                for (uint64_t i = start_seq; i < start_seq + info.data_cnt; ++i) {
                    Packet* p = cache_.FindPacket(i);
                    if (p) {
                        rtn.push_back(const_cast<DataPacket*>(&(p->data_packet())));
                    }
                }
                next_submit_seq_ = start_seq + info.TotalCount();
                last_submit_time_ = timestamp_ms();
            } else {
                // 2. 有可能只是剩下的未接收到, 不用管， 等待接收后续的包即可
            }
            
        }
    }

    
}
