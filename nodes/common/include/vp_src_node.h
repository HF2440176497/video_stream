

#pragma once

#include <string>

#include "excepts/vp_excepts.h"
#include "nodes/common/include/vp_node.h"
#include "nodes/common/include/vp_stream_info_hookable.h"
#include "nodes/source/vp_data_source.h"
#include "utils/vp_gate.h"
#include "utils/pybind_py_caller.h"

namespace vp_nodes {
class vp_src_node : public vp_node, public vp_stream_info_hookable {

public:
  vp_src_node(std::string node_name, int channel_index, float resize_ratio = 1.0);
  virtual ~vp_src_node();
  void start();
  void stop();
  void speak();

  virtual void handle_run() override;
  virtual void deinitialized() override;
  virtual void handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) override;

 public:
  utils::vp_gate gate;
  std::shared_ptr<vp_sources::vp_data_source> data_source_;
  
 private:
  int channel_index = -1;
  vp_stream_info stream_info;

 private:
  std::shared_ptr<utils::PythonMethod> py_call;
   
};

}  // namespace vp_nodes
