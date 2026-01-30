
if(NOT DEFINED VIDEO_STREAM_ROOT_PATH)
    set(VIDEO_STREAM_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..
        CACHE INTERNAL "Top-level directory of this project")
endif()

if(NOT has_build_gtest)
  add_subdirectory(${VIDEO_STREAM_ROOT_PATH}/3rdparty/googletest)
  set(has_build_gtest ON)
endif()
include_directories(${VIDEO_STREAM_ROOT_PATH}/3rdparty/googletest/googletest/include)

include_directories(
  ${VIDEO_STREAM_ROOT_PATH}
)

set(test_srcs "main.cpp")

add_executable(video_stream_modules_test ${test_srcs})
add_dependencies(video_stream_modules_test video_stream)
target_link_libraries(video_stream_modules_test 
  video_stream
  Python3::Python
  gflags 
  glog 
  gtest 
  rt 
  dl 
  pthread
)

