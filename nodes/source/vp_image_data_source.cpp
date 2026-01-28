
#include <filesystem>

#include "nodes/source/vp_image_data_source.h"

#include "utils/logging.h"
#include "utils/vp_utils.h"

namespace vp_sources {

vp_image_data_source::vp_image_data_source(int channel_index, std::string file_path, 
    int interval, float resize_ratio, bool cycle)
    : vp_data_source(channel_index, resize_ratio),
      file_path(file_path),
      interval(interval),
      cycle(cycle),
      from_file(true) {
  is_opened = false;
  assert(interval >= 1 && interval <= 60);
}

vp_image_data_source::~vp_image_data_source() { close(); }

bool vp_image_data_source::open() {
  if (is_opened) {
    return true;
  }
  // 查看文件是否存在
  if (!std::filesystem::exists(file_path)) {
    LOGE(SOURCE) << "Image file not exist: " << file_path;
    return false;
  }
  is_opened = true;
  return true;
}

void vp_image_data_source::close() { is_opened = false; }

bool vp_image_data_source::read(cv::Mat& frame) {
  if (!is_opened) {
    return false;
  }
  this->cache_frame = cv::imread(file_path);
  if (this->cache_frame.empty()) {
    LOGE(SOURCE) << "Failed to read image: " << file_path;
    return false;
  }
  // 复制到传入的参数
  frame = this->cache_frame.clone();
  return true;
}

std::string vp_image_data_source::get_info() const { return file_path; }
}  // namespace vp_sources
