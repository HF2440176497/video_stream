
#pragma once


#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <memory>
#include <chrono>

#include "logging.h"

#include "nodes/common/include/vp_meta_publisher.h"
#include "nodes/common/include/vp_meta_hookable.h"

#include "utils/safe_queue.h"  // thread safe queue
#include "utils/vp_semaphore.h"  // thread safe semaphore

namespace vp_nodes {

enum class vp_node_type {
  SRC,
  MID,
  DES
};

/**
 * @brief 节点基类，包含通用的功能，涵盖源节点、中间节点、目的节点
 */
class vp_node_base : public vp_meta_hookable {

 protected:
  vp_node_base() = default;
  vp_node_base(std::string node_name): node_name_(node_name) {}
  virtual ~vp_node_base() = default;
  inline vp_node_type node_type() const { return node_type_; }
  virtual void handle_run();
  virtual void dispatch_run();
  virtual void initialized() = 0;  // wait for thread start
  virtual void deinitialized() = 0;  // wait for thread join

 protected:
  utils::ThreadSafeQueue<std::shared_ptr<vp_objects::vp_meta>> in_queue;  // cache input meta from previous nodes
  utils::ThreadSafeQueue<std::shared_ptr<vp_objects::vp_meta>> out_queue;  // cache output meta to next nodes
  utils::vp_semaphore in_queue_semaphore;  // synchronize for in_queue
  utils::vp_semaphore out_queue_semaphore;  // synchronize for out_queue

 public:
  std::thread handle_thread;
  std::thread dispatch_thread;
  vp_node_type node_type_ = vp_node_type::MID;
  std::string node_name_;
  virtual std::string to_string() { return node_name_; };  // describe node info
};

/**
 * @brief 中间节点基类
 */
class vp_node : public vp_meta_publisher,
                public vp_meta_subscriber,
                public vp_node_base,
                public std::enable_shared_from_this<vp_node> {
 private:
  // previous nodes
  std::vector<std::shared_ptr<vp_node_base>> pre_nodes;

 protected:
  bool alive = true;
  int frame_meta_handle_batch = 1;

  /**
   * @brief 处理单帧元数据，默认实现为直接返回元数据
   * 
   * @param meta 输入帧元数据
   * @return std::shared_ptr<vp_objects::vp_meta> 输出帧元数据，返回nullptr表示不将元数据推送到下一个节点
   */
  virtual std::shared_ptr<vp_objects::vp_meta> handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta);

  /**
   * @brief 处理单帧控制元数据，默认实现为直接返回元数据
   * 
   * @param meta 输入控制元数据
   * @return std::shared_ptr<vp_objects::vp_meta> 输出控制元数据，返回nullptr表示不将元数据推送到下一个节点
   */
  virtual std::shared_ptr<vp_objects::vp_meta> handle_control_meta(std::shared_ptr<vp_objects::vp_control_meta> meta);

  /**
   * @brief 处理批量帧元数据，默认实现为直接返回元数据
   * 
   * @param meta_with_batch 输入批量帧元数据, size maybe one
   */
  virtual void handle_frame_meta(const std::vector<std::shared_ptr<vp_objects::vp_frame_meta>>& meta_with_batch);

  virtual void initialized() override;
  virtual void deinitialized() override;

  /**
   * @brief 将元数据直接送入缓存队列，等待被处理线程处理，而非直接推送给下一节点
   * 适用于在节点线程内，发送处理后的帧数据/控制数据
   * 
   * @param meta 输入元数据
   */
  void pending_meta(std::shared_ptr<vp_objects::vp_meta> meta);

  /**
   * @brief constructor
   * 
   * @param node_name 节点名称
   */
  vp_node(std::string node_name);

 public:
  virtual ~vp_node();

  // receive meta from previous nodes,
  // we can hook(such as modifying meta) in child class but do not forget calling vp_node.meta_flow(...) followly.
  virtual void meta_flow(std::shared_ptr<vp_objects::vp_meta> meta) override;

  // detach myself from all previous nodes
  void detach();

  // detach myself from specific previous nodes
  void detach_from(std::vector<std::string> pre_node_names);

  // detach myself from all previous nodes AND the same action on all next nodes(recursively), can be used to split the
  // whole pipeline into single nodes before process exits.
  void detach_recursively();

  // attach myself to previous nodes(can be a list)
  void attach_to(std::vector<std::shared_ptr<vp_node_base>> pre_nodes);

  // get next nodes
  std::vector<std::shared_ptr<vp_node_base>> next_nodes();
};

}  // namespace vp_nodes