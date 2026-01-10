
#include <iterator>

#include "objects/include/vp_frame_meta.h"


namespace vp_objects {

vp_frame_meta::vp_frame_meta(cv::Mat frame, int frame_index, int channel_index, 
                            int original_width, int original_height, int fps)
    : vp_meta(vp_meta_type::FRAME, channel_index),
      frame_index_(frame_index),
      original_width_(original_width),
      original_height_(original_height),
      fps_(fps),
      frame_(frame) {
  assert(!frame_.empty());
}

// copy constructor of vp_frame_meta would NOT be called at most time.
// only when it flow through vp_split_node with vp_split_node::split_with_deep_copy==true.
// in fact, all kinds of meta would NOT be copyed in its lifecycle, we just pass them by poniter most time.
vp_frame_meta::vp_frame_meta(const vp_frame_meta& meta)
    : vp_meta(meta),
      frame_index_(meta.frame_index_),
      original_width_(meta.original_width_),
      original_height_(meta.original_height_),
      fps_(meta.fps_) {
  // deep copy frame data
  this->frame_ = meta.frame_.clone();
}

vp_frame_meta::~vp_frame_meta() {}

std::shared_ptr<vp_meta> vp_frame_meta::clone() {
  // just call copy constructor and return new pointer
  return std::make_shared<vp_frame_meta>(*this);
}

}  // namespace vp_objects