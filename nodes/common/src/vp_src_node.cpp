

#include "nodes/common/include/vp_src_node.h"
#include "nodes/source/vp_image_data_source.h"



namespace vp_nodes {

vp_src_node::vp_src_node(std::string node_name, int channel_index, float resize_ratio)
    : vp_node(node_name), channel_index(channel_index) {
  node_type_ = vp_node_type::SRC;
  data_source_ = std::make_shared<vp_sources::vp_image_data_source>(channel_index, "TEST.png");
  py_call = std::make_shared<utils::PythonMethod>();
  assert(resize_ratio > 0 && resize_ratio <= 1.0f);
  initialized();  // not override
}

vp_src_node::~vp_src_node() {
  if (data_source_) {
    data_source_->close();
  }
  deinitialized();
  py_call.reset();  // 确保 join 完成后再析构 caller
}

void vp_src_node::deinitialized() {
  gate.open();
  vp_node::deinitialized();
}

/**
 * @note 实现与 data_source_ 的基本交互, 不从 in_queue 获取数据
 */
void vp_src_node::handle_run() {
  if (!data_source_) {
    LOGE(NODE) << "data_source_ is not initialized!";
    return;
  }
  if (!data_source_->is_open()) {
    if (!data_source_->open()) {
      LOGE(NODE) << "data_source_ open failed!";
      return;
    }
    LOGI(NODE) << "data_source_ open success!";
  }
  while (alive && data_source_->is_open()) {
    gate.knock();
    if (!alive) break;

    auto frame_meta = data_source_->get_frame_meta();
    if (!frame_meta) {
      LOGE(NODE) << "get_frame_meta failed!";
      break;
    }
    this->out_queue.Push(frame_meta);
    handle_frame_meta(frame_meta);
    this->out_queue_semaphore.signal();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));  // TODO:
  }
  this->out_queue.Push(nullptr);
  this->out_queue_semaphore.signal();
  LOGE(NODE) << "handle_run exit!";
}

/**
 * 尝试在这里进行 PY 交互
 */
void vp_src_node::handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) {
  LOGI(SOURCE) << "------------ handle_frame_meta, frame_index: " << meta->frame_index_;
  meta = py_call->process(meta, "python_scripts.process_frame");
  LOGI(SOURCE) << "  after process, frame_index: " << meta->frame_index_;
}

void vp_src_node::start() { gate.open(); }

void vp_src_node::stop() { gate.close(); }

void vp_src_node::speak() {
  auto speak_control_meta = std::make_shared<vp_objects::vp_control_meta>(vp_objects::vp_control_type::SPEAK, this->channel_index);
  this->push_meta(speak_control_meta);
}

}  // namespace vp_nodes