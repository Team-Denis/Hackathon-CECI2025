cmake_minimum_required(VERSION 3.10)
project(HackathonCECI2025)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenSSL for MD5 hashing
find_package(OpenSSL REQUIRED)

# Find libqrencode for QR code generation
find_package(PkgConfig REQUIRED)
pkg_check_modules(QRENCODE REQUIRED libqrencode)

# Source files
set(SOURCES
        src/main.cpp
        src/FormatManager/DenisDecoder.cpp
        src/FormatManager/DenisEncoder.cpp
        src/FormatManager/FileManagementHelper.cpp
        src/PhysicalStorage/CryptoUtils.cpp
        src/PhysicalStorage/MatrixChunker.cpp
        src/PhysicalStorage/QRCodeStorage.cpp
        src/PhysicalStorage/QRCodeVisualizer.cpp
)

# Add executable
add_executable(HackathonCECI2025 ${SOURCES})

# Include directories - the main include directory only
target_include_directories(HackathonCECI2025 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link against OpenSSL and QRencode
target_link_libraries(HackathonCECI2025 PRIVATE
        OpenSSL::Crypto
        ${QRENCODE_LIBRARIES}
)

# Add QRencode include directories
target_include_directories(HackathonCECI2025 PRIVATE ${QRENCODE_INCLUDE_DIRS})

# Add QRencode library directories
link_directories(${QRENCODE_LIBRARY_DIRS})

# Find or fetch raylib
include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.5.0
)
# Configure raylib to build as a static library
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# Link against raylib
target_link_libraries(HackathonCECI2025 PRIVATE raylib)

# Find and link against the filesystem library for older GCC versions
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(HackathonCECI2025 PRIVATE stdc++fs)
endif ()

# Installation target
install(TARGETS HackathonCECI2025 DESTINATION bin)