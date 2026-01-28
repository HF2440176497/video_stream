
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>
#include <opencv2/opencv.hpp>

#include "utils/pybind_vp_meta.h"

namespace py = pybind11;

namespace utils {

py::array_t<uint8_t> to_numpy(cv::Mat& mat) {
  if (mat.empty()) {
    return py::array_t<uint8_t>();
  }
  std::vector<size_t> shape = {static_cast<size_t>(mat.rows), static_cast<size_t>(mat.cols),
                               static_cast<size_t>(mat.channels())};
  std::vector<size_t> strides = {static_cast<size_t>(mat.step[0]), static_cast<size_t>(mat.step[1]), sizeof(uint8_t)};

  return py::array_t<uint8_t>(shape,         // 形状 (H, W, C)
                              strides,       // 步长
                              mat.data,      // 数据指针
                              py::cast(mat)  // 保持Mat生命周期（防止GC）
  );
}

cv::Mat to_mat(py::array_t<uint8_t> input) {
  py::buffer_info info = input.request();

  if (info.ndim != 2 && info.ndim != 3) {
    throw std::runtime_error("Only 2D or 3D numpy arrays supported");
  }
  int rows = info.shape[0];
  int cols = info.shape[1];
  int channels = (info.ndim == 3) ? info.shape[2] : 1;

  int type = (channels == 1) ? CV_8UC1 : (channels == 3) ? CV_8UC3 : CV_8UC4;
  return cv::Mat(rows, cols, type, info.ptr);
}
};  // namespace utils