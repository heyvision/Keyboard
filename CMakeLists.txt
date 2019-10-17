cmake_minimum_required(VERSION 3.1.0)
project(keyboard)

set (CMAKE_CXX_STANDARD 11)

set(OpenCV_DIR /home/hey/Append/opencv4/build)
FIND_PACKAGE(OpenCV REQUIRED)

include_directories(./include ./include/Components)
set(SRC_LIST  ./src/Components/RealSense.cpp ./src/ImageProc.cpp)
add_executable(keyboard main.cpp ${SRC_LIST})

target_link_libraries(keyboard ${DEPENDENCIES} ${OpenCV_LIBS} realsense2 X11)


