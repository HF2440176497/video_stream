
#include "objects/include/vp_frame_meta.h"

#include "utils/pybind_vp_meta.h"  // cv::Mat convert


PYBIND11_MODULE(video_stream, m) {
    m.doc() = "Video Processing Pipeline Python Bindings";

    py::enum_<vp_objects::vp_meta_type>(m, "vp_meta_type")
        .value("FRAME", vp_objects::vp_meta_type::FRAME)
        .value("CONTROL", vp_objects::vp_meta_type::CONTROL)
        .export_values();  // 导出到模块层级，允许直接访问 FRAME/CONTROL
    
    py::class_<vp_objects::vp_meta, std::shared_ptr<vp_objects::vp_meta>>(m, "vp_meta")
        .def_readwrite("meta_type", &vp_objects::vp_meta::meta_type_)
        .def_readwrite("channel_index", &vp_objects::vp_meta::channel_index_)
        .def("clone", &vp_objects::vp_meta::clone);

    py::class_<vp_objects::vp_frame_meta, vp_objects::vp_meta, std::shared_ptr<vp_objects::vp_frame_meta>>(m, "vp_frame_meta")
        .def(py::init<>())
        .def(py::init<cv::Mat, int, int, int, int, int>(),
             py::arg("frame"), py::arg("frame_index") = -1,
             py::arg("channel_index") = -1, py::arg("original_width") = 0,
             py::arg("original_height") = 0, py::arg("fps") = 0)
        
        .def_readwrite("frame_index", &vp_objects::vp_frame_meta::frame_index_)
        .def_readwrite("fps", &vp_objects::vp_frame_meta::fps_)
        .def_readwrite("original_width", &vp_objects::vp_frame_meta::original_width_)
        .def_readwrite("original_height", &vp_objects::vp_frame_meta::original_height_)
        
        .def_property("frame", 
            &vp_objects::vp_frame_meta::get_frame_numpy,   // getter
            &vp_objects::vp_frame_meta::set_frame_numpy,   // setter
            "Frame data as numpy array (zero-copy)")
        
        .def("get_size", &vp_objects::vp_frame_meta::get_size)
        .def("clone", &vp_objects::vp_frame_meta::clone)
        .def("__repr__", &vp_objects::vp_frame_meta::__repr__);
        
    m.def("mat_to_numpy", &utils::to_numpy, "Convert cv::Mat to numpy array");
    m.def("numpy_to_mat", &utils::to_mat, "Convert numpy array to cv::Mat");
}