
if(NOT DEFINED VIDEO_STREAM_ROOT_PATH)
    set(VIDEO_STREAM_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..
        CACHE INTERNAL "Top-level directory of this project")
endif()

include(${VIDEO_STREAM_ROOT_PATH}/cmake/have_video_stream_target.cmake)

if(NOT build_gtest_already)
  add_subdirectory(${VIDEO_STREAM_ROOT_PATH}/3rdparty/googletest)
  set(build_gtest_already ON)
endif()
include_directories(${VIDEO_STREAM_ROOT_PATH}/3rdparty/googletest/googletest/include)

include_directories(
  ${VIDEO_STREAM_ROOT_PATH}
)

# set(test_srcs "")

# have_video_stream_target(${VIDEO_STREAM_ROOT_PATH})

# add_executable(video_stream_modules_test ${test_srcs})
# add_dependencies(video_stream_modules_test video_stream)
# target_link_libraries(video_stream_modules_test video_stream gflags glog gtest rt dl pthread)


