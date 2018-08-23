cmake_minimum_required(VERSION 3.12)
project(micro_cpp)

set(CMAKE_CXX_STANDARD 14)

# BOOST
set(BOOST_ROOT "/usr/local/Cellar/boost/1.67.0")
find_package(Boost 1.67.0 COMPONENTS REQUIRED system filesystem thread log program_options chrono regex)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif()

set (Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set (Boost_USE_MULTITHREAD ON)  # enable multithreading

include_directories(${Boost_INCLUDE_DIRS})
# END BOOST


add_executable(micro_cpp main.cpp src/base_ctrl.cpp src/base_ctrl.h)