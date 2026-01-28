#pragma once

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>
#include <opencv2/opencv.hpp>

namespace py = pybind11;

namespace utils {

py::array_t<uint8_t> to_numpy(cv::Mat& mat);

cv::Mat to_mat(py::array_t<uint8_t> input);

};  // namespace utils