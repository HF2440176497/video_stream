#pragma once

#include <condition_variable>
#include <mutex>

namespace utils {
// semaphore used to resume/pause loop structure, it blocks thread while received unactive signal and unblock thread
// while received active signal. refer to vp_semaphore also
class vp_gate {
 public:
  vp_gate() { opened_ = false; }

  // wait until opened
  void knock() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [=] { return opened_; });
  }

  // try to open the gate, to avoid knock() block
  void open() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      opened_ = true;
    }
    cv_.notify_one();
  }

  void close() {
    std::lock_guard<std::mutex> lock(mutex_);
    opened_ = false;
  }

  bool is_open() {
    std::lock_guard<std::mutex> lock(mutex_);
    return opened_;
  }

 private:
  std::mutex              mutex_;
  std::condition_variable cv_;
  bool                    opened_;
};
}  // namespace utils