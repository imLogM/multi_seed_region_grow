cmake_minimum_required(VERSION 3.8)
project(seed_grow)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wextra -pedantic -Wno-long-long")
endif()
set(CMAKE_CXX_STANDARD 11)

set(OpenCV_DIR /home/logm/opencv-2.4.13.6/build/install/share/OpenCV)   # set opencv dir
FIND_PACKAGE( OpenCV 2 REQUIRED )

set(OpenCV_LIB_DIR /home/logm/opencv-2.4.13.6/build/install/lib)    # set opencv library dir
link_directories ( ${OpenCV_LIB_DIR} )
MESSAGE(STATUS "OpenCV_LIB_DIR: ${OpenCV_LIB_DIR}")
MESSAGE(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")

include_directories ( ${OpenCV_INCLUDE_DIRS} )      # set opencv include dir
MESSAGE(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")

include_directories(${CMAKE_SOURCE_DIR})

FILE(GLOB_RECURSE sourcefiles "${CMAKE_SOURCE_DIR}/main.cpp")

add_executable(example ${sourcefiles})
target_link_libraries(example ${OpenCV_LIBS})
