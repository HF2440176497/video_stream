#pragma once

#include <memory>
#include <opencv2/opencv.hpp>
#include <string>

#include "objects/include/vp_frame_meta.h"

namespace vp_sources {

class vp_data_source {
 protected:
  int         channel_index;
  bool        is_opened;

  int   frame_index;
  int   video_width;
  int   video_height;
  int   fps;
  float resize_ratio;

 public:
  vp_data_source(int channel_index, float resize_ratio = 1.0f);
  virtual ~vp_data_source() = default;

  bool                is_open() const;
  virtual bool        open() = 0;
  virtual void        close() = 0;
  virtual std::string get_info() const = 0;

  int get_channel_index() const;
  int get_frame_index() const;
  int get_video_width() const;
  int get_video_height() const;
  int get_fps() const;

  virtual bool                               read(cv::Mat& frame) = 0;
  std::shared_ptr<vp_objects::vp_frame_meta> get_frame_meta();

 protected:
  cv::Mat cache_frame;
};
}  // namespace vp_sources
