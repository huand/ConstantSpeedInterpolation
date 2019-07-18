cmake_minimum_required(VERSION 3.14)
project( ImageInterp )
find_package( OpenCV REQUIRED )
# add_executable( ImageInterp src/interp2frames.cc )
# target_link_libraries( ImageInterp ${OpenCV_LIBS} )

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable( IdInterp src/FrameIdInterp.cc )

add_executable( VideoInterp
src/VideoInterp.cc
src/interp2frames.cc
)
target_link_libraries( VideoInterp ${OpenCV_LIBS} )
