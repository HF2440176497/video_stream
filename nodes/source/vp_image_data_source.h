#pragma once

#include <string>
#include <opencv2/opencv.hpp>

#include "nodes/source/vp_data_source.h"

namespace vp_sources {

class vp_image_data_source : public vp_data_source {
 private:
  std::string      file_path;
  int              interval;
  bool             cycle;
  bool             from_file;

 public:
  vp_image_data_source(int channel_index, std::string file_path, 
                       int interval = 1, 
                       float resize_ratio = 1.0f,
                       bool cycle = true);
  virtual ~vp_image_data_source() override;

  virtual bool        open() override;
  virtual void        close() override;
  virtual bool        read(cv::Mat& frame) override;
  virtual std::string get_info() const override;
};
}  // namespace vp_sources
