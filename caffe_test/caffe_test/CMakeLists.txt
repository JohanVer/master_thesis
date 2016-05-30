project(caffe_test)
cmake_minimum_required(VERSION 3.1)

add_definitions(-DCPU_ONLY)
add_definitions(${Caffe_DEFINITIONS})

set(GLOG_LIB "/usr/lib/x86_64-linux-gnu/libglog.so")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_BUILD_TYPE Debug)

find_package( Caffe REQUIRED )
find_package( OpenCV 3.1 REQUIRED )
find_package(Boost COMPONENTS system filesystem chrono REQUIRED)

list(APPEND CMAKE_MODULE_PATH "/home/vertensj/caffe/caffe/cmake/Modules")
find_package( Glog REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} ${Caffe_INCLUDE_DIRS} ${Glog_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}  include )

add_library(caffe_test
   src/classify.cpp
   include/classify.h
 )

target_link_libraries ( caffe_test  ${OpenCV_LIBS} ${GLOG_LIB} ${Boost_LIBRARIES} ${Caffe_LIBRARIES} )

add_executable(caffe_test_ex src/main.cpp)
target_link_libraries (caffe_test_ex caffe_test)

