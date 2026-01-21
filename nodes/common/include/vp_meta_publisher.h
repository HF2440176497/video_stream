#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include "nodes/common/include/vp_meta_subscriber.h"

namespace vp_nodes {
class vp_meta_publisher {
 private:
 protected:
  // push meta to next nodes
  // 因此如果此节点后面存在下游节点，那么就需要继承这个 publisher，自己作为发送者
  virtual void push_meta(std::shared_ptr<vp_objects::vp_meta> meta);

  // non-copyable for all child class
  std::mutex subscribers_lock;
  // next nodes as subscribers
  std::vector<std::shared_ptr<vp_meta_subscriber>> subscribers;

 public:
  vp_meta_publisher(/* args */);
  virtual ~vp_meta_publisher();

  // non-copyable
  vp_meta_publisher(const vp_meta_publisher&) = delete;
  vp_meta_publisher& operator=(const vp_meta_publisher&) = delete;

  // add next node
  void add_subscriber(std::shared_ptr<vp_meta_subscriber> subscriber);
  // remove next node
  void remove_subscriber(std::shared_ptr<vp_meta_subscriber> subscriber);
};

}  // namespace vp_nodes
