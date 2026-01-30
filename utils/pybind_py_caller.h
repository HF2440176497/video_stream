
#pragma once

#include <Python.h>
#include <pybind11/embed.h>

#include "objects/include/vp_frame_meta.h"
#include "utils/logging.h"

namespace py = pybind11;

namespace utils {

class PythonMethod {
 private:
  py::scoped_interpreter guard_{};  // Python解释器生命周期管理
  std::unique_ptr<py::gil_scoped_release> release_gil_;

 public:
  PythonMethod() {
    {
      py::gil_scoped_acquire ensure;
      py::module sys = py::module::import("sys");
      sys.attr("path").attr("append")(PROJECT_ROOT);
      LOGI(PY) << "Python initialized, sys.path appended";
    }
    release_gil_ = std::make_unique<py::gil_scoped_release>();
  }
  PythonMethod(const PythonMethod&) = delete;
  PythonMethod& operator=(const PythonMethod&) = delete;
  PythonMethod(PythonMethod&&) = delete;
  PythonMethod& operator=(PythonMethod&&) = delete;

  ~PythonMethod() {
    if (release_gil_) {
      release_gil_.reset();  // 重新获取 GIL
    }
  }

  /**
   * @brief 调用指定的 Python 脚本处理当前帧
   */
  std::shared_ptr<vp_objects::vp_frame_meta> process(std::shared_ptr<vp_objects::vp_frame_meta> meta,
                                               const std::string&             python_module,
                                               const std::string&             python_func = "process_frame") {
    std::shared_ptr<vp_objects::vp_frame_meta> result_meta;
    {
        // std::cerr << "PythonMethod::process: Py_IsInitialized() = " << Py_IsInitialized() << std::endl;  // should == 1
        py::gil_scoped_acquire acquire;
        try {
            py::module script = py::module::import(python_module.c_str());
            py::object func = script.attr(python_func.c_str());
            py::object result = func(meta);
            result_meta = result.is_none() 
                ? meta 
                : result.cast<std::shared_ptr<vp_objects::vp_frame_meta>>();
        } catch (py::error_already_set& e) {
            std::cerr << "Python error: " << e.what() << std::endl;
            result_meta = meta;
        }
    }
    return result_meta;
  }
};  // end class PythonMethod

}  // namespace utils
