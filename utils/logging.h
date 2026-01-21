
#pragma once

#include <glog/logging.h>

#define LOGF(tag) LOG(FATAL) << "[VIDEO_STREAM " << (#tag) << " FATAL] "
#define LOGE(tag) LOG(ERROR) << "[VIDEO_STREAM " << (#tag) << " ERROR] "
#define LOGW(tag) LOG(WARNING) << "[VIDEO_STREAM " << (#tag) << " WARN] "
#define LOGI(tag) LOG(INFO) << "[VIDEO_STREAM " << (#tag) << " INFO] "
#define LOGD(tag) VLOG(1) << "[VIDEO_STREAM " << (#tag) << " DEBUG] "
#define LOGT(tag) VLOG(2) << "[VIDEO_STREAM " << (#tag) << " TRACE] "

#define LOGF_IF(tag, condition) LOG_IF(FATAL, condition) << "[VIDEO_STREAM " << (#tag) << " FATAL] "
#define LOGE_IF(tag, condition) LOG_IF(ERROR, condition) << "[VIDEO_STREAM " << (#tag) << " ERROR] "
#define LOGW_IF(tag, condition) LOG_IF(WARNING, condition) << "[VIDEO_STREAM " << (#tag) << " WARN] "
#define LOGI_IF(tag, condition) LOG_IF(INFO, condition) << "[VIDEO_STREAM " << (#tag) << " INFO] "
#define LOGD_IF(tag, condition) VLOG_IF(1, condition) << "[VIDEO_STREAM " << (#tag) << " DEBUG] "
#define LOGT_IF(tag, condition) VLOG_IF(2, condition) << "[VIDEO_STREAM " << (#tag) << " TRACE] "
