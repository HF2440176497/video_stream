

#include "nodes/common/include/vp_src_node.h"

#include "excepts/vp_excepts.h"

namespace vp_nodes {

vp_src_node::vp_src_node(std::string node_name, int channel_index, float resize_ratio)
    : vp_node_base(node_name), channel_index(channel_index), resize_ratio(resize_ratio), frame_index(-1) {
  node_type_ = vp_node_type::SRC;
  assert(resize_ratio > 0 && resize_ratio <= 1.0f);
}

void vp_src_node::handle_run() {
  throw vp_excepts::vp_not_implemented_error("must have re-implementaion for 'handle_run' method in src nodes!");
}

std::shared_ptr<vp_objects::vp_meta> vp_src_node::handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) {
  throw vp_excepts::vp_invalid_calling_error("'handle_frame_meta' method could not be called in src nodes!");
}

std::shared_ptr<vp_objects::vp_meta> vp_src_node::handle_control_meta(
    std::shared_ptr<vp_objects::vp_control_meta> meta) {
  throw vp_excepts::vp_invalid_calling_error("'handle_control_meta' method could not be called in src nodes!");
}


void vp_src_node::initialized() {
  handle_thread = std::thread(&vp_src_node::handle_run, this);
  dispatch_thread = std::thread(&vp_src_node::dispatch_run, this);
}

/**
 * @brief deinitialized the src node 析构函数中调用
 */
void vp_src_node::deinitialized() {
  alive = false;
  gate.open();
  if (handle_thread.joinable()) {
    handle_thread.join();
  }
  if (dispatch_thread.joinable()) {
    dispatch_thread.join();
  }
}

void vp_src_node::start() { gate.open(); }

void vp_src_node::stop() { gate.close(); }

}  // namespace vp_nodes