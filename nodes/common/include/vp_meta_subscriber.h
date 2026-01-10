#pragma once

#include <memory>

#include "objects/include/vp_meta.h"

namespace vp_nodes {
class vp_meta_subscriber {
 private:
  /* data */
 public:
  vp_meta_subscriber(/* args */);
  virtual ~vp_meta_subscriber();

  // non-copyable for all child class
  vp_meta_subscriber(const vp_meta_subscriber&) = delete;
  vp_meta_subscriber& operator=(const vp_meta_subscriber&) = delete;

  // receive meta from previous nodes，involved by previous node (as publisher)
  virtual void meta_flow(std::shared_ptr<vp_objects::vp_meta> meta) = 0;
};

}  // namespace vp_nodes
