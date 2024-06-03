#include <glog/logging.h>
#include "util/ge_channel.h"
#include "util/timestamp.h"

int main(int argc, char** argv) {
    DCHECK_EQ(argc, 6);
    google::InitGoogleLogging(argv[1]);

    // 设置日志文件输出目录
    FLAGS_log_dir = "/app/proj/data_analysis";
    FLAGS_minloglevel = google::INFO;

    // 可选：设置日志输出级别，默认为 INFO
    GEChannel channel(std::atof(argv[2]), std::atof(argv[3]), std::atof(argv[4]), std::atof(argv[5]), time(NULL));
    uint64_t now = timestamp_ms();
    uint64_t duration = 60 * 1000;

    while (timestamp_ms() - now < duration) {
        LOG(INFO) << !channel.IsLossThisRound();
        channel.Transition();
    }
    google::ShutdownGoogleLogging();
}