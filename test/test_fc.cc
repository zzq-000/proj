#include <random>
#include "flow_controller.h"
#include "glog/logging.h"

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = "./"; 
    FlowController fc;
    srand(1000);
    while (true) {
        int size = rand() % 1000 + 1000;
        if (fc.CanSend(size)) {
            LOG(INFO) << "send: " << timestamp_ms() << " " << size; 
        } else {
            LOG(WARNING) << "miss: " << timestamp_ms() << " " << size; 

        }
    }
}