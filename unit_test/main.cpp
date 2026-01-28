
#include <thread>
#include <glog/logging.h>

#include "nodes/common/include/vp_src_node.h"


int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_minloglevel = 0;
    FLAGS_logtostderr = 1;

    auto image_src_node = std::make_shared<vp_nodes::vp_src_node>("image_src_node", 0, 1.0f);
    image_src_node->start();
    std::this_thread::sleep_for(std::chrono::seconds(1000));
    image_src_node->stop();

    google::ShutdownGoogleLogging();
}

