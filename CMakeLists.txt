cmake_minimum_required(VERSION 3.10)
project(HackathonCECI2025)

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

    src/FormatManager/DenisEncoder.cpp
    src/FormatManager/DenisDecoder.cpp
    src/FormatManager/FileManagementHelper.cpp

    src/PhysicalStorage/chunker.cpp
    src/PhysicalStorage/storage.cpp
    src/PhysicalStorage/utils.cpp
    src/PhysicalStorage/visualization.cpp
    src/PhysicalStorage/common.cpp
)

# Add executable
add_executable(HackathonCECI2025 ${SOURCES})

# Link against OpenSSL
target_link_libraries(HackathonCECI2025 PRIVATE OpenSSL::Crypto)

# Include the include directory for the target
target_include_directories(HackathonCECI2025 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find or fetch raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 4.5.0 # Replace with the desired version
)
FetchContent_MakeAvailable(raylib)

# Link against raylib
target_link_libraries(HackathonCECI2025 PRIVATE raylib)

# Find and link against the filesystem library
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(HackathonCECI2025 PRIVATE stdc++fs)
endif()

# Installation target
install(TARGETS HackathonCECI2025 DESTINATION bin)