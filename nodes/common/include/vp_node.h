
#pragma once


#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <memory>
#include <chrono>

#include "objects/include/vp_frame_meta.h"
#include "objects/include/vp_control_meta.h"

#include "nodes/common/include/vp_meta_publisher.h"
#include "nodes/common/include/vp_meta_hookable.h"

#include "utils/logging.h"
#include "utils/safe_queue.h"  // thread safe queue
#include "utils/vp_semaphore.h"  // thread safe semaphore

namespace vp_nodes {

enum class vp_node_type {
  SRC,
  MID,
  DES
};

/**
 * @brief 中间节点基类
 */
class vp_node : public vp_meta_publisher,
                public vp_meta_subscriber,
                public vp_meta_hookable,
                public std::enable_shared_from_this<vp_node> {
 public:
  std::string node_name_;
  vp_node_type node_type_ = vp_node_type::MID;
  std::thread handle_thread;
  std::thread dispatch_thread;
  std::vector<std::shared_ptr<vp_node>> pre_nodes;

 protected:
  virtual std::shared_ptr<vp_objects::vp_meta> handle_control_meta(std::shared_ptr<vp_objects::vp_control_meta> meta);
  virtual void handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta);
  virtual void handle_frame_meta(const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& meta_with_batch);

  virtual void handle_run();
  virtual void dispatch_run();
  void initialized();
  virtual void deinitialized();
  virtual void meta_flow(std::shared_ptr<vp_objects::vp_meta> meta);  // receive meta from previous nodes,
  
  std::vector<std::shared_ptr<vp_node>> next_nodes();
  void detach();  // detach myself from all previous nodes
  void detach_from(std::vector<std::string> pre_node_names);  // detach myself from specific previous nodes
  void detach_recursively();  // detach myself from all previous nodes AND the same action on all next nodes(recursively),
  void attach_to(std::vector<std::shared_ptr<vp_node>> pre_nodes);  // attach myself to nodes(can be a list)

  /**
   * @brief 将元数据直接送入缓存队列，等待被处理线程处理，而非直接推送给下一节点
   * 适用于在节点线程内，发送处理后的帧数据/控制数据
   */
  void pending_meta(std::shared_ptr<vp_objects::vp_meta> meta);

 public:
  vp_node() = default;
  vp_node(std::string node_name);
  virtual ~vp_node();
  inline vp_node_type node_type() { return node_type_; };
  virtual std::string to_string() { return node_name_; };  // describe node info
  
 protected:
  bool alive = true;
  int frame_meta_handle_batch = 1;
  utils::ThreadSafeQueue<std::shared_ptr<vp_objects::vp_meta>> in_queue;  // cache input meta from previous nodes
  utils::ThreadSafeQueue<std::shared_ptr<vp_objects::vp_meta>> out_queue;  // cache output meta to next nodes
  utils::vp_semaphore in_queue_semaphore;  // synchronize for in_queue
  utils::vp_semaphore out_queue_semaphore;  // synchronize for out_queue
};

}  // namespace vp_nodes