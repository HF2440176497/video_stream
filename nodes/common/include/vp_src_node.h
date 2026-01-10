

#pragma once

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "utils/logging.h"
#include "nodes/common/include/vp_data_source.h"
#include "nodes/common/include/vp_meta_hookable.h"
#include "nodes/common/include/vp_meta_publisher.h"
#include "nodes/common/include/vp_node.h"

namespace vp_nodes {

/**
 * @brief source node, publish meta to next nodes
 * Has DataSource memeber to get frame
 */
class vp_src_node : public vp_meta_publisher,
                    public vp_meta_hookable,
                    public vp_node_base,
                    public std::enable_shared_from_this<vp_src_node> {
 protected:
  // DataSource need to reimplment how to get frame with data_source_; but reuse dispatch_run to push frame
  virtual void                                 handle_run() override;

  // must reimplement in child class of vp_src_node
  virtual std::shared_ptr<vp_objects::vp_meta> handle_frame_meta(std::shared_ptr<vp_objects::vp_frame_meta> meta);
  virtual std::shared_ptr<vp_objects::vp_meta> handle_control_meta(std::shared_ptr<vp_objects::vp_control_meta> meta);

 protected:
  vp_src_node() = default;
  vp_src_node(std::string node_name, int channel_index, float resize_ratio = 1.0);
  virtual ~vp_src_node() = default;

  int original_fps = -1;
  int original_width = 0;
  int original_height = 0;
  
  int   frame_index;
  int   channel_index;
  float resize_ratio = 1.0;

  std::shared_ptr<vp_data_source> data_source_;
  vp_utils::vp_gate               gate;

 protected:
  // override from vp_node_base
  virtual void initialized() override;
  virtual void deinitialized() override;
};

}  // namespace vp_nodes
