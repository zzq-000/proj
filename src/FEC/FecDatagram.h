#pragma once
#include <string>

#include <glog/logging.h>
#include "fec_type.pb.h"


struct FecInfo{
    int data_cnt;
    int redundancy_cnt;
    std::string type;
    inline int TotalCount() const {
        return data_cnt + redundancy_cnt;
    }
};



FecInfo GetInfoAboutFEC(FecType t) {
    switch (t)
    {
    case FecType::FEC_NONE:
        return {0, 0, "FEC_none"};
    case FecType::FEC_2_1:
        return {2, 1, "FEC_2_1"};
    case FecType::FEC_3_1:
        return {3, 1, "FEC_3_1"};
    case FecType::FEC_3_2:
        return {3, 2, "FEC_3_2"};
    case FecType::FEC_4_1:
        return {4, 1, "FEC_4_1"};
    case FecType::FEC_4_2:
        return {4, 2, "FEC_4_2"};
    case FecType::FEC_4_3:
        return {4, 3, "FEC_4_3"};
    case FecType::FEC_5_1:
        return {5, 1, "FEC_5_1"};
    case FecType::FEC_5_2:
        return {5, 2, "FEC_5_2"};
    case FecType::FEC_5_3:
        return {5, 3, "FEC_5_3"};
    case FecType::FEC_5_4:
        return {5, 4, "FEC_5_4"};
    case FecType::FEC_6_1:
        return {6, 1, "FEC_6_1"};
    case FecType::FEC_6_2:
        return {6, 2, "FEC_6_2"};
    case FecType::FEC_6_3:
        return {6, 3, "FEC_6_3"};
    case FecType::FEC_6_4:
        return {6, 4, "FEC_6_4"};
    case FecType::FEC_7_1:
        return {7, 1, "FEC_7_1"};
    default:
        LOG(ERROR) << "no such fec_type";
        return {};
    }
}