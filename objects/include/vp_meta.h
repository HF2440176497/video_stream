
#pragma once

#include <assert.h>

#include <any>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vp_objects {

// meta type
enum vp_meta_type { FRAME, CONTROL };

// meta trace field
// 1. sequence   ->int       ,sequence number the meta flowing through pipeline
// 2. node_name  ->string    ,name of current node the meta flow through
// 3. in_time    ->long      ,time when the meta arrive current node
// 4. out_time   ->long      ,time when the meta leave current node
// 5. text_info  ->vector    ,text info while the meta inside node
enum vp_meta_trace_field { SEQUENCE, NODE_NAME, IN_TIME, OUT_TIME, TEXT_INFO };

// base class for meta
class vp_meta {
 private:
 public:
  vp_meta() = default;
  vp_meta(vp_meta_type meta_type, int channel_index);
  vp_meta(const vp_meta& other) = default;
  virtual ~vp_meta() = default;

  std::chrono::system_clock::time_point create_time_;
  vp_meta_type meta_type_;

  std::string task_;
  int channel_index_;
  bool trace_on = false;

  virtual std::string get_traces_str();
  virtual std::string get_meta_str();

  // virtual clone method since we do not know what specific meta we need copy in some situations, return a new pointer
  // pointting to new memory allocation in heap. note: every child class need implement its own clone() method.
  virtual std::shared_ptr<vp_meta> clone() = 0;
};

}  // namespace vp_objects