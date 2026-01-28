
#pragma once

#include <pybind11/embed.h>
#include "objects/include/vp_frame_meta.h"

namespace py = pybind11;

namespace utils {

class PythonMethod {
 private:
  py::scoped_interpreter guard_{};  // Python解释器生命周期管理

 public:
  PythonMethod() {
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("append")(PROJECT_ROOT);
  }
  PythonMethod(const PythonMethod&) = delete;
  PythonMethod& operator=(const PythonMethod&) = delete;
  PythonMethod(PythonMethod&&) = delete;
  PythonMethod& operator=(PythonMethod&&) = delete;

  ~PythonMethod() = default;

  /**
   * @brief 调用指定的 Python 脚本处理当前帧
   */
  std::shared_ptr<vp_objects::vp_frame_meta> process(std::shared_ptr<vp_objects::vp_frame_meta> meta,
                                               const std::string&             python_module,
                                               const std::string&             python_func = "process_frame") {
    py::gil_scoped_acquire acquire;
    try {
      py::module script = py::module::import(python_module.c_str());
      py::object func = script.attr(python_func.c_str());
      py::object result = func(meta);
      if (result.is_none()) {
        return meta;  // Python修改了原对象
      } else {
        return result.cast<std::shared_ptr<vp_objects::vp_frame_meta>>();
      }
    } catch (py::error_already_set& e) {
      std::cerr << "Python error: " << e.what() << std::endl;
      return meta;
    }
  }
};  // end class PythonMethod

}  // namespace utils
