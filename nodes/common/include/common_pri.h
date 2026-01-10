#pragma once

#include <string.h>
#include <unistd.h>


/*!
 * @class NonCopyable
 *
 * @brief NonCopyable is the abstraction of the class which has no ability to do copy and assign. It is always be used
 *        as the base class to disable copy and assignment.
 */
class NonCopyable {
 protected:
  /*!
   * @brief Constructs an instance with empty value.
   *
   * @param None.
   *
   * @return  None.
   */
  NonCopyable() = default;
  /*!
   * @brief Destructs an instance.
   *
   * @param None.
   *
   * @return  None.
   */
  ~NonCopyable() = default;

 private:
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable(NonCopyable&&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
  NonCopyable& operator=(NonCopyable&&) = delete;
};  // end NonCopyable