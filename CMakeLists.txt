cmake_minimum_required(VERSION 3.10)
project(FileChunker)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenSSL for MD5 hashing
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
        src/main.cpp
        src/chunker.cpp
        src/utils.cpp
)

# Add executable
add_executable(file_chunker ${SOURCES})

# Link against OpenSSL
target_link_libraries(file_chunker PRIVATE OpenSSL::Crypto)

# Find and link against the filesystem library
# This is needed for older compilers where std::filesystem
# was still experimental or needed linking against stdc++fs
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(file_chunker PRIVATE stdc++fs)
endif ()

# Installation target
install(TARGETS file_chunker DESTINATION bin)