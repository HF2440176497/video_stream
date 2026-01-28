
#include <iterator>

#include "objects/include/vp_frame_meta.h"
#include "utils/pybind_vp_meta.h"

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

std::shared_ptr<vp_meta> vp_frame_meta::clone() {
  // just call copy constructor and return new pointer
  return std::make_shared<vp_frame_meta>(*this);
}

// ----------------- 用于绑定

py::array_t<uint8_t> vp_frame_meta::get_frame_numpy() {
    return utils::to_numpy(frame_);
}

void vp_frame_meta::set_frame_numpy(py::array_t<uint8_t> arr, bool copy = false) {
    cv::Mat new_mat = utils::to_mat(arr);
    frame_ = copy ? new_mat.clone() : new_mat;
}

std::tuple<int, int> vp_frame_meta::get_size() const {
    return {frame_.cols, frame_.rows};
}

std::string vp_frame_meta::__repr__() const {
    return "vp_frame_meta(idx=" + std::to_string(frame_index_) + 
            ", size=" + std::to_string(frame_.cols) + "x" + 
            std::to_string(frame_.rows) + ")";
}

}  // namespace vp_objects