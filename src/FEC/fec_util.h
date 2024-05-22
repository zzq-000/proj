#ifndef UTIL_H
#define UTIL_H


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



FecInfo GetInfoAboutFEC(FecType t);
#endif // UTIL_H