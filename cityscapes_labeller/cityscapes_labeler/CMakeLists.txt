project(cityscapes_labeller)
cmake_minimum_required(VERSION 3.1)

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

find_package(Boost COMPONENTS system filesystem chrono REQUIRED)
find_package( OpenCV 3.1 REQUIRED )

set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.6/gcc_64)
set(CMAKE_INCLUDE_PATH $ENV{HOME}/Qt/5.6/gcc_64)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets)

QT5_WRAP_UI(QT_FORMS_HPP ${QT_FORMS} display_window.ui)

include_directories(${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )

add_library(cityscapes_lib
    display_window.cpp
    cityscapes_manager.cpp
    cityscapes_widgets.cpp
    ${QT_FORMS_HPP}
    display_window.h
    cityscapes_manager.h
    cityscapes_widgets.h
    )

target_link_libraries(cityscapes_lib ${Boost_LIBRARIES} ${OpenCV_LIBS} Qt5::Widgets)

add_executable(cityscapes_labeller main.cpp)
target_link_libraries(cityscapes_labeller cityscapes_lib Qt5::Widgets)
