macro(have_framework_target video_stream_root_path)
  if(TARGET video_stream_core)
    set(HAVE_FRAMEWORK_TARGET TRUE)
  else()
    if(NOT EXISTS ${video_stream_root_path}/lib/libvideo_stream_core.so)
      message(FATAL_ERROR "Build framework first")
    endif()
    link_directories(${video_stream_root_path}/lib)
  endif()
  include_directories(${video_stream_root_path}/framework/core/include)
endmacro()



