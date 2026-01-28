
#include "nodes/common/include/vp_node.h"
#include "utils/vp_utils.h"
#include "utils/logging.h"
#include "excepts/vp_excepts.h"


namespace vp_nodes {

vp_node::vp_node(std::string node_name) : node_name_(node_name) {
  node_type_ = vp_node_type::MID;
}

vp_node::~vp_node() {
  deinitialized();
}

void vp_node::handle_run() {
  // cache for batch handling if need
  std::vector<std::shared_ptr<vp_objects::vp_frame_meta>> frame_meta_batch_cache {};
  while (alive) {
    this->in_queue_semaphore.wait();
    LOGD(utils::string_format("[%s] before handling meta, in_queue.size()==>%d", node_name_.c_str(), in_queue.Size()));
    
    std::shared_ptr<vp_objects::vp_meta> in_meta;
    in_queue.WaitAndPop(in_meta);

    // dead flag
    if (in_meta == nullptr) {
      continue;
    }
    // handling hooker activated if need
    invoke_meta_handling_hooker(node_name_, in_queue.Size(), in_meta);

    LOGD(NODE) << utils::string_format("[%s] after handling meta, in_queue.size()==>%d", node_name_.c_str(), in_queue.Size());

    std::shared_ptr<vp_objects::vp_meta> out_meta;
    auto                                 batch_complete = false;

    // call handlers
    if (in_meta->meta_type_ == vp_objects::vp_meta_type::CONTROL) {
      auto control_meta = std::dynamic_pointer_cast<vp_objects::vp_control_meta>(in_meta);
      out_meta = this->handle_control_meta(control_meta);
    } else if (in_meta->meta_type_ == vp_objects::vp_meta_type::FRAME) {
      auto frame_meta = std::dynamic_pointer_cast<vp_objects::vp_frame_meta>(in_meta);
      frame_meta_batch_cache.push_back(frame_meta);
      if (frame_meta_batch_cache.size() >= frame_meta_handle_batch) {
        this->handle_frame_meta(frame_meta_batch_cache);
        batch_complete = true;
      } else {
        LOGD(NODE) << utils::string_format("[%s] handle meta with batch, frame_meta_batch_cache.size()==>%d",
                                          node_name_.c_str(), frame_meta_batch_cache.size());
      }
    } else {
      throw "invalid meta type!";
    }

    // one by one mode
    // return nullptr means do not push it to next nodes(such as in des nodes).
    if (in_meta->meta_type_ == vp_objects::vp_meta_type::CONTROL && out_meta == nullptr) {
      frame_meta_batch_cache.clear();
      continue;
    }
    // 处理完后送入缓存队列 out_queue
    if (batch_complete) {
      for (auto& i : frame_meta_batch_cache) {
        LOGD(NODE) << utils::string_format("[%s] before handling meta, out_queue.size()==>%d", node_name_.c_str(), out_queue.Size());
        this->out_queue.Push(i);
        invoke_meta_handled_hooker(node_name_, out_queue.Size(), i);
        this->out_queue_semaphore.signal();
        LOGD(NODE) << utils::string_format("[%s] after handling meta, out_queue.size()==>%d", node_name_.c_str(), out_queue.Size());
      }
      frame_meta_batch_cache.clear();
    }
  }  // end while(alive)
  // send dead flag for dispatch_thread
  this->out_queue.Push(nullptr);
  this->out_queue_semaphore.signal();
}

void vp_node::dispatch_run() {
  while (alive) {
    this->out_queue_semaphore.wait();
    LOGD(NODE) << utils::string_format("[%s] before dispatching meta, out_queue.size()==>%d",
                node_name_.c_str(), out_queue.Size());
    auto out_meta = this->out_queue.Front();
    if (out_meta == nullptr) {
      continue;
    }
    invoke_meta_leaving_hooker(node_name_, out_queue.Size(), out_meta);
    this->push_meta(out_meta);
    this->out_queue.Pop();  // cache queue derectly pop out
    LOGD(NODE) << utils::string_format("[%s] after dispatching meta, out_queue.size()==>%d", 
                node_name_.c_str(), out_queue.Size());
  }
}

void vp_node::handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta) {
  return;
}

void vp_node::handle_frame_meta(const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& meta_with_batch) {
  for (auto& i : meta_with_batch) {
    this->handle_frame_meta(i);
  }
}

std::shared_ptr<vp_objects::vp_meta> vp_node::handle_control_meta(std::shared_ptr<vp_objects::vp_control_meta> meta) {
  return meta;
}

// override vp_meta_subscriber::meta_flow
// invoked by pre_nodes
void vp_node::meta_flow(std::shared_ptr<vp_objects::vp_meta> meta) {
  if (meta == nullptr) {
    return;
  }
  LOGD(NODE) << utils::string_format("[%s] before meta flow, in_queue.size()==>%d", 
                node_name_.c_str(), in_queue.Size());
  this->in_queue.Push(meta);
  invoke_meta_arriving_hooker(node_name_, in_queue.Size(), meta);
  this->in_queue_semaphore.signal();
  LOGD(NODE) << utils::string_format("[%s] after meta flow, in_queue.size()==>%d", node_name_.c_str(), in_queue.Size());
}

// 从所有的前一个节点的订阅者移除当前节点, 上游节点不再会发送数据给当前节点，当前节点呈现 “detached” 状态
void vp_node::detach() {
  for (auto i : this->pre_nodes) {
    if (i->node_type() == vp_node_type::DES) {  // should not be des
      throw vp_excepts::vp_invalid_argument_error("DES nodes not in pre_nodes");
    }
    i->remove_subscriber(shared_from_this());
  }
  this->pre_nodes.clear();
}

// 在 pre_nodes 成员中查找存在于输入列表中的节点，然后移除当前节点作为 pre_node 的订阅
void vp_node::detach_from(std::vector<std::string> pre_node_names) {
  for (auto i = this->pre_nodes.begin(); i != this->pre_nodes.end();) {
    if (std::find(pre_node_names.begin(), pre_node_names.end(), (*i)->node_name_) != pre_node_names.end()) {
      (*i)->remove_subscriber(shared_from_this());
      i = this->pre_nodes.erase(i);
    } else {
      i++;
    }
  }
}

void vp_node::detach_recursively() {
  detach();
  auto nodes = next_nodes();
  for (auto& n : nodes) {
    n->detach_recursively();
  }
}

/**
 * @brief 将自己作为 pre_nodes 各个节点的订阅者
 * pre_nodes 不应当存在 des node
 */ 
void vp_node::attach_to(std::vector<std::shared_ptr<vp_node>> pre_nodes) {
  if (this->node_type() == vp_node_type::SRC) {  // should not be src
    throw vp_excepts::vp_invalid_argument_error("SRC nodes must not have any pre nodes!");
  }
  for (auto i : pre_nodes) {
    i->add_subscriber(shared_from_this());
    if (i->node_type() == vp_node_type::DES) {  // should not be des
      throw vp_excepts::vp_invalid_argument_error("DES nodes must not have any next nodes!");
    }
    this->pre_nodes.push_back(i);
  }
}

void vp_node::initialized() {
  // start threads since all resources have been initialized
  this->handle_thread = std::thread(&vp_node::handle_run, this);
  this->dispatch_thread = std::thread(&vp_node::dispatch_run, this);
}

void vp_node::deinitialized() {
  alive = false;
  this->in_queue.Push(nullptr);
  this->in_queue_semaphore.signal();
  // wait for threads exits in vp_node
  if (handle_thread.joinable()) {
    handle_thread.join();
  }
  if (dispatch_thread.joinable()) {
    dispatch_thread.join();
  }
}

std::vector<std::shared_ptr<vp_node>> vp_node::next_nodes() {
  std::vector<std::shared_ptr<vp_node>> next_nodes;
  std::lock_guard<std::mutex>           guard(this->subscribers_lock);
  for (auto& i : this->subscribers) {
    next_nodes.push_back(std::dynamic_pointer_cast<vp_node>(i));
  }
  return next_nodes;
}

void vp_node::pending_meta(std::shared_ptr<vp_objects::vp_meta> meta) {
  this->out_queue.Push(meta);
  invoke_meta_handled_hooker(node_name_, out_queue.Size(), meta);
  this->out_queue_semaphore.signal();
}

}  // namespace vp_nodes