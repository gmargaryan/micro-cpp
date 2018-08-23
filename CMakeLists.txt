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

# OpenSSL
set(OPENSSL_ROOT_DIR "/usr/local/Cellar/openssl/*")
set(OPENSSL_INCLUDE_DIR "/usr/local/Cellar/openssl/1.0.2p/include")
set(OPENSSL_LIBRARIES "/usr/local/Cellar/openssl/1.0.2p/lib")
find_package(OpenSSL 1.0.2 REQUIRED)

if(NOT OPENSSL_FOUND)
    message(FATAL_ERROR "Could not find OpenSSL!")
endif()

set(OPENSSL_LIBS "/usr/local/Cellar/openssl/${OPENSSL_VERSION}/lib/libssl.1.0.0.dylib;/usr/local/Cellar/openssl/${OPENSSL_VERSION}/lib/libcrypto.1.0.0.dylib")
# END OpenSSL


# COMPILER CONFIGURATION
set(CMAKE_CXX_FLAGS "-arch x86_64 -v -std=c++11 -stdlib=libc++ -DBOOST_LOG_DYN_LINK -Wno-deprecated-declarations -I/usr/local/Cellar/openssl/1.0.2p/include/")
# END

add_executable(micro_cpp main.cpp src/base_ctrl.cpp src/base_ctrl.h)

set(LIBRARIES_SEARCH_PATHS ${OPENSSL_LIBS} ${Boost_LIBRARIES})

target_link_libraries (micro_cpp ${Boost_LIBRARIES} ${OPENSSL_LIBS})