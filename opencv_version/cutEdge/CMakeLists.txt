cmake_minimum_required(VERSION 3.0)
project( ce )
ADD_DEFINITIONS(-DENABLE_DEBUG -g -std=c++11)
find_package( OpenCV REQUIRED )
add_executable( ce  main.cpp )
target_link_libraries( ce ${OpenCV_LIBS} )
