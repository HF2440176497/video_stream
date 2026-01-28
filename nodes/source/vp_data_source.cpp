#include "nodes/source/vp_data_source.h"

#include "utils/vp_utils.h"

namespace vp_sources {

vp_data_source::vp_data_source(int channel_index, float resize_ratio)
    : channel_index(channel_index),
      resize_ratio(resize_ratio),
      is_opened(false),
      frame_index(-1),
      video_width(-1),
      video_height(-1),
      fps(-1) {
  assert(resize_ratio > 0 && resize_ratio <= 1.0f);
}

bool vp_data_source::is_open() const { return is_opened; }

int vp_data_source::get_channel_index() const { return channel_index; }

int vp_data_source::get_frame_index() const { return frame_index; }

int vp_data_source::get_video_width() const { return video_width; }

int vp_data_source::get_video_height() const { return video_height; }

int vp_data_source::get_fps() const { return fps; }

std::shared_ptr<vp_objects::vp_frame_meta> vp_data_source::get_frame_meta() {
  cv::Mat frame;
  if (!read(frame)) {
    return nullptr;
  }
  cv::Mat resize_frame;
  if (resize_ratio != 1.0f) {
    cv::resize(frame, resize_frame, cv::Size(), resize_ratio, resize_ratio);
  } else {
    resize_frame = frame.clone();
  }
  frame_index++;
  video_width = resize_frame.cols;
  video_height = resize_frame.rows;
  auto meta = std::make_shared<vp_objects::vp_frame_meta>(resize_frame, frame_index, channel_index,
                                                        video_width, video_height, fps);
  return meta;
}
}  // namespace vp_sources
