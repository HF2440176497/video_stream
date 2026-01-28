
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "Please create a separate directory for build files.")
endif()

cmake_minimum_required(VERSION 3.13)

project(video_stream LANGUAGES C CXX)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(build_tests "build unit tests" ON)
option(build_tools "build tools" OFF)
option(build_samples "build sample programs" OFF)

option(BUILD_GFLAGS "Build gflags library" ON)
option(BUILD_GLOG "Build glog library" ON)
option(BUILD_LIBYUV "Build libyuv library" ON)
option(BUILD_NLOHMAN_JSON "Build nlohmann json library" ON)

option(NVIDIA_PLATFORM "NVIDIA platform (Enable CUDA)" OFF)
option(RKNN_PLATFORM "RKNN platform (Enable RKNN)" OFF)

if(NVIDIA_PLATFORM)
  find_package(CUDA REQUIRED)
  enable_language(CUDA)
  set(CMAKE_CUDA_STANDARD 17)
  set(CMAKE_CUDA_STANDARD_REQUIRED ON)
  add_compile_definitions(NVIDIA_PLATFORM)
  message(STATUS "CUDA support enabled - CUDA version: ${CMAKE_CUDA_COMPILER_VERSION}")
elseif(RKNN_PLATFORM)
  # ...

  add_compile_definitions(RKNN_PLATFORM)
  message(STATUS "RKNN support enabled")
endif()

set(VIDEO_STREAM_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}"
    CACHE INTERNAL "Top-level directory of this project")

add_definitions(-DPROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
  
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VIDEO_STREAM_ROOT_PATH}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VIDEO_STREAM_ROOT_PATH}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${VIDEO_STREAM_ROOT_PATH}/bin)

if(build_tests)
  add_definitions(-DUNIT_TEST)
endif()

if(build_tests)
  # ---[ gtest
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest)

  # get_target_property(GTEST_LOCATION gtest LOCATION)
  # message(STATUS "GTest library location: ${GTEST_LOCATION}")
  
  # get_target_property(GTEST_TYPE gtest TYPE)
  # message(STATUS "GTest target type: ${GTEST_TYPE}")

  set(has_build_gtest ON)  # used by subdirectories
endif()


# ---[ libyuv
if(BUILD_LIBYUV)
  set(LIBYUV_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})  # 与项目保持一致
  set(LIBYUV_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  set(LIBYUV_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libyuv)
  message(STATUS "libyuv library built and integrated")
endif()

if (BUILD_NLOHMAN_JSON)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nlohmann-json)
endif()

set(3RDPARTY_LIBS "")
set(DEPENDENCIES "")  # dependencies which manually compiled 

# ---[ opencv
find_package(OpenCV REQUIRED COMPONENTS 
    core imgproc highgui features2d imgcodecs videoio)
if(OpenCV_FOUND)
  list(APPEND 3RDPARTY_LIBS ${OpenCV_LIBS})
  include_directories(${OpenCV_INCLUDE_DIRS})
  message(STATUS "OpenCV version: ${OpenCV_VERSION}; Include dirs: ${OpenCV_INCLUDE_DIRS}; Libraries: ${OpenCV_LIBS}")
else()
  message(FATAL_ERROR "OpenCV not found! Please ensure OpenCV is installed and set in CMAKE_PREFIX_PATH")
endif()


# ---[ yuv
if(BUILD_LIBYUV)
  message(STATUS "Using libyuv library from root build ")
  include_directories(${VIDEO_STREAM_ROOT_PATH}/3rdparty/libyuv/include)
  if(TARGET yuv)  # 根据 libyuv 内的 cmake 文件确定
    list(APPEND 3RDPARTY_LIBS yuv)
    list(APPEND DEPENDENCIES yuv)
    message(STATUS "----- Using libyuv static library: yuv")
    message(STATUS "----- Library path: $<TARGET_FILE:yuv>")
  elseif(TARGET yuv_shared)
    list(APPEND 3RDPARTY_LIBS yuv_shared)
    list(APPEND DEPENDENCIES yuv_shared)
    message(STATUS "----- Using libyuv shared library: yuv_shared")
    message(STATUS "----- Library path: $<TARGET_FILE:yuv_shared>")
  endif()
else()
  message(FATAL_ERROR "libyuv not found. Set BUILD_LIBYUV=ON in root CMakeLists.txt to build")
endif()

# ---[ gflags
include(${VIDEO_STREAM_ROOT_PATH}/cmake/FindGFlags.cmake)
include_directories(${GFLAGS_INCLUDE_DIRS})
list(APPEND 3RDPARTY_LIBS ${GFLAGS_LIBRARY})

# ---[ glog
include(${VIDEO_STREAM_ROOT_PATH}/cmake/FindGlog.cmake)
include_directories(${GLOG_INCLUDE_DIRS})
list(APPEND 3RDPARTY_LIBS ${GLOG_LIBRARY})

# ---[ pybind11
find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS})
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/pybind11)

find_package(Python3 COMPONENTS Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})

# ---[ Include directories
include_directories(
  ${VIDEO_STREAM_ROOT_PATH}
)

# ---[ Collect all source files
file(GLOB_RECURSE VIDEO_STREAM_SOURCES
  "${VIDEO_STREAM_ROOT_PATH}/nodes/*/*.cpp"
  "${VIDEO_STREAM_ROOT_PATH}/objects/*/*.cpp"
  "${VIDEO_STREAM_ROOT_PATH}/utils/src/*.cpp"
  "${VIDEO_STREAM_ROOT_PATH}/excepts/*.cpp"
  "${VIDEO_STREAM_ROOT_PATH}/py/*/*.cpp"
)

# ---[ Build video_stream dynamic library
add_library(video_stream SHARED ${VIDEO_STREAM_SOURCES})
# pybind11_add_module(video_stream SHARED ${VIDEO_STREAM_SOURCES})

# Set dependencies to ensure third-party libraries are built first
target_link_libraries(video_stream PRIVATE 
  ${3RDPARTY_LIBS} 
  pybind11::module
  Python3::Python
  )
add_dependencies(video_stream ${DEPENDENCIES})

message(STATUS "Video stream library will be built as: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libvideo_stream.so")

if(build_tests)
  add_subdirectory(unit_test)
endif()

