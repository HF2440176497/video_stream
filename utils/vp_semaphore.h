
#pragma once

#include <condition_variable>
#include <mutex>

namespace utils {

// semaphore for queue/deque data structures in VideoPipe, used for producer-consumer pattern.
// it blocks the consumer thread until data has come.
// Sasha: lock_guard to protect count_
class vp_semaphore {
 public:
  vp_semaphore() { count_ = 0; }

  void signal() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      ++count_;
    }
    cv_.notify_one();
  }

  void wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [=] { return count_ > 0; });
    --count_;
  }

  void reset() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      count_ = 0;
    }
    cv_.notify_one();
  }

 private:
  std::mutex              mutex_;
  std::condition_variable cv_;
  int                     count_;
};
}  // namespace utils
