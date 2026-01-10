#pragma once

#include <chrono>

#include "objects/include/vp_meta.h"

namespace vp_objects {
// type of control meta
enum vp_control_type { SPEAK, VIDEO_RECORD, IMAGE_RECORD };

// control meta, which contains control data.
class vp_control_meta : public vp_meta {
 private:
  // help to generate control uid if need
  void generate_uid();

 public:
  vp_control_meta(vp_control_type control_type, int channel_index, std::string control_uid = "");
  virtual ~vp_control_meta() = default;
  vp_control_meta(const vp_control_meta& other) = default;

  vp_control_type                  control_type;
  std::string                      control_uid;
  virtual std::shared_ptr<vp_meta> clone() override;
};

}  // namespace vp_objects