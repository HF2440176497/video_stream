
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <vector>

#include "objects/include/vp_meta.h"

namespace vp_objects {

// frame meta, which contains frame-related data. it is kind of important meta in pipeline.
class vp_frame_meta : public vp_meta {
 private:
  /* data */
 public:
  vp_frame_meta(cv::Mat frame, int frame_index = -1, int channel_index = -1, 
                int original_width = 0,
                int original_height = 0, 
                int fps = 0);
  virtual ~vp_frame_meta() = default;
  vp_frame_meta(const vp_frame_meta& meta);

  // frame the meta belongs to, filled by src nodes.
  int frame_index_;

  // fps for current video.
  int fps_;

  // orignal frame width, fiiled by src nodes.
  int original_width_;

  // original frame height, filled by src nodes.
  int original_height_;

  // image data the meta holds, filled by src nodes.
  // deep copy needed here for this member.
  cv::Mat frame_;

  // copy myself
  virtual std::shared_ptr<vp_meta> clone() override;
};

}  // namespace vp_objects
