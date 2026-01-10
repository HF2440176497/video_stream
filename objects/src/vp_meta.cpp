
#include "objects/include/vp_meta.h"


namespace vp_objects {

vp_meta::vp_meta(vp_meta_type meta_type, int channel_index) : meta_type_(meta_type), channel_index_(channel_index) {
  create_time_ = std::chrono::system_clock::now();
}

std::string vp_meta::get_traces_str() { return ""; }

std::string vp_meta::get_meta_str() { return ""; }

}  // namespace vp_objects