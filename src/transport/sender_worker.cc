#include "sender_worker.h"
#include "util/timestamp.h"

SWorker::SWorker(Config config)
    :cache_(), config_(config), seq_num_(0), codec_(), to_fec_encode_() {
}

Packet* SWorker::RegisterDataPacket(const DataPacket& packet) {
    // 使用 unique_ptr 来管理动态分配的内存
    std::unique_ptr<DataPacket> data = std::make_unique<DataPacket>(packet);
    Packet* p = cache_.GetNextPlaceToCache();
    
    // 将所有权转移给 Protobuf 对象
    p->set_allocated_data_packet(data.release());
    p->set_seq_num(seq_num_++);
    return p;
}

Packet* SWorker::RegisterDataPacket(void* buffer, int len) {
    Packet* p = cache_.GetNextPlaceToCache();
    std::unique_ptr<DataPacket> data = std::make_unique<DataPacket>();
    data->set_len(len);
    data->set_data(buffer, len);
    p->set_allocated_data_packet(data.release());
    p->set_seq_num(seq_num_++);
    return p;
}

//注意是对DataPacket做冗余， 不是对Packet做冗余
void SWorker::EncodeFecOnce(std::list<Packet*>& rtn) {

    Packet* p = NULL;
    // 对于to_fec_encode_中存储的序列号，但无法在cache_中保存的包， 进行清理
    while(!to_fec_encode_.empty() && (p = cache_.FindPacket(to_fec_encode_.front().second)) == NULL) {
        LOG(INFO) << "cache_ has free the packet, cannot find packet: " << seq_num_;
        to_fec_encode_.pop_front();
    }

    if (to_fec_encode_.empty()) return;
    
    FecType type = p->fec_type();
    FecInfo info = GetInfoAboutFEC(type);

    int data_packet_num = std::min(info.data_cnt, static_cast<int>(to_fec_encode_.size()));
    // LOG(INFO) << "data_packet_num: " << data_packet_num; 
    std::vector<void*> buffer(info.TotalCount());
    std::vector<Packet*> packets(info.TotalCount());
    int encode_size = 0;
    
    // 获取encode_size
    for (int i = 0; i < data_packet_num; ++i) {

        uint64_t seq_num = to_fec_encode_.front().second;
        to_fec_encode_.pop_front();
        Packet* p = cache_.FindPacket(seq_num);
        
        // 如果p真的不见了？
        assert(p != NULL);
        p->set_fec_type(type);
        p->set_fec_index(i);
        packets[i] = p;
        // LOG(INFO) << (int)p->data_packet().ByteSizeLong() << " " << p->seq_num() << " " << i;
        encode_size = std::max(encode_size, (int)p->data_packet().ByteSizeLong());
    }
    
    // 如果DataPacket数量不够， 生成空包
    for (int i = data_packet_num; i < info.data_cnt; ++i) {
        Packet* p = RegisterDataPacket(DataPacket());
        p->set_fec_type(type);
        p->set_fec_index(i);
        packets[i] = p;
    }

    uint8_t* allocated_array = new uint8_t[encode_size * info.TotalCount()];
    // 准备FEC encode数据
    for (int i = 0; i < info.TotalCount(); ++i) {
        buffer[i] = allocated_array + encode_size;
        if (i < info.data_cnt) {
            assert(packets.at(i)->data_packet().ByteSizeLong() <= encode_size);
            packets.at(i)->data_packet().SerializeToArray(buffer[i], packets.at(i)->data_packet().ByteSizeLong());
        }
    }

    codec_.Encode(buffer, type, encode_size);
    // LOG(INFO) << "encode_size: " << encode_size;
    
    // 无法直接将Encode的数据直接作为DataPacket发送出去
    // 因为DataPacket.ParseFromArray()有可能失败？ TODO, 即便成功了，也要在DataPacket中添加一个长度， max_length, 提供给接收端作为decode_size
    // 因此直接将编码后的数组作为DataPacket的data字段， len设置为长度
    // 这样接收端就可以直接从冗余数据包中获取decode_length
    for (int i = info.data_cnt; i < info.TotalCount(); ++i) {
        Packet* p = RegisterDataPacket(buffer[i], encode_size);
        p->set_fec_index(i);
        p->set_fec_type(type);
        packets[i] = p;
    }

    for(int i = 0; i < packets.size(); ++i) {
        rtn.push_back(packets[i]);
    }
    delete[] allocated_array;

}



void SWorker::ClearFec(std::list<Packet*>& rtn) {
    if (to_fec_encode_.empty()) return;
    while (!to_fec_encode_.empty()) {
        EncodeFecOnce(rtn);
    }
}


std::list<Packet*> SWorker::RegisterPackets(const DataPacket& packet) {
    std::list<Packet*> rtn;
    
    if (config_.fec_type == FecType::FEC_NONE) {
        ClearFec(rtn); // 如果还有等待FEC编码的包， 立即编码发送出去
        Packet* p = RegisterDataPacket(packet);
        p->set_fec_type(FecType::FEC_NONE);
        // 不需要设置fec_index
        rtn.push_back(p);
    }
    else {

        Packet* p = RegisterDataPacket(packet);
        p->set_fec_type(config_.fec_type);
        FecInfo info = GetInfoAboutFEC(config_.fec_type);
        to_fec_encode_.emplace_back(timestamp_ms(), p->seq_num());

        while (!to_fec_encode_.empty() &&
             (to_fec_encode_.size() >= info.data_cnt
                 || timestamp_ms() - to_fec_encode_.front().first >= config_.fec_encode_wait_time_limit_ms)
                 ) {
            EncodeFecOnce(rtn);
        }
    }
    return rtn;
}