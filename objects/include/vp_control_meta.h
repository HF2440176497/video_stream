#pragma once

#include <chrono>

#include "objects/include/vp_meta.h"

namespace vp_objects {
// type of control meta
enum vp_control_type { SPEAK, VIDEO_RECORD, IMAGE_RECORD };

class vp_control_meta : public vp_meta {
 private:
  void generate_uid();

 public:
  vp_control_meta(vp_control_type control_type, int channel_index, std::string control_uid = "");
  virtual ~vp_control_meta() = default;
  virtual std::shared_ptr<vp_meta> clone() override;

  std::string     control_uid;
  vp_control_type control_type;
};

}  // namespace vp_objects