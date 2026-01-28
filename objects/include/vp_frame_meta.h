
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "objects/include/vp_meta.h"

namespace py = pybind11;

namespace vp_objects {

// frame meta, which contains frame-related data. it is kind of important meta in pipeline.
class vp_frame_meta : public vp_meta {
 private:
  /* data */
 public:
  vp_frame_meta() = default;
  vp_frame_meta(cv::Mat frame, int frame_index = -1, int channel_index = -1, 
                int original_width = 0,
                int original_height = 0, 
                int fps = 0);
  virtual ~vp_frame_meta() = default;
  vp_frame_meta(const vp_frame_meta& meta);

  int frame_index_;
  int fps_;
  int original_width_;
  int original_height_;
  cv::Mat frame_;
  virtual std::shared_ptr<vp_meta> clone() override;

  py::array_t<uint8_t> get_frame_numpy();
  void                 set_frame_numpy(py::array_t<uint8_t> arr, bool copy);
  std::tuple<int, int> get_size() const;
  std::string          __repr__() const;
};

}  // namespace vp_objects
