macro(have_video_stream_target video_stream_root_path)
  if(TARGET video_stream_core)
    set(HAVE_VIDEO_STREAM_TARGET TRUE)
  else()
    if(NOT EXISTS ${video_stream_root_path}/lib/libvideo_stream.so)
      message(FATAL_ERROR "Build video_stream first")
    endif()
    link_directories(${video_stream_root_path}/lib)
  endif()
endmacro()



