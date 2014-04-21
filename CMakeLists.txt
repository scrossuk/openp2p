# --- Script Setup

cmake_minimum_required (VERSION 2.8)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "In-source builds are not allowed.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# Disable in-source builds and modifications
# to the source tree.
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Enable compiler tests.
enable_testing()

project(OpenP2P C CXX)

if(NOT CMAKE_BUILD_TYPE)
	# Build debug by default.
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type (options are None, Debug, Release, RelWithDebInfo and MinSizeRel)." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# --- Compiler Properties

set(OPENP2P_MAJOR_VERSION 1)
set(OPENP2P_MINOR_VERSION 0)
set(OPENP2P_PATCH_VERSION 0)
set(OPENP2P_BUILD_VERSION 0)
set(OPENP2P_VERSION ${OPENP2P_MAJOR_VERSION}.${OPENP2P_MINOR_VERSION}.${OPENP2P_PATCH_VERSION}.${OPENP2P_BUILD_VERSION})

message(STATUS "Building OpenP2P version ${OPENP2P_VERSION} using build type '${CMAKE_BUILD_TYPE}'.")
message(STATUS "    Source directory is '${PROJECT_SOURCE_DIR}'.")
message(STATUS "    Build directory is '${PROJECT_BINARY_DIR}'.")

# --- Compiler Flags

add_definitions( -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS )

# Enable most warnings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wshadow -Wundef -Wpointer-arith -Wcast-align -Wwrite-strings")

# Use C++11.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Enable/disable optimisation depending on build type.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")

# Add version as preprocessor defines.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPENP2P_VERSION=${OPENP2P_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPENP2P_MAJOR_VERSION=${OPENP2P_MAJOR_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPENP2P_MINOR_VERSION=${OPENP2P_MINOR_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPENP2P_PATCH_VERSION=${OPENP2P_PATCH_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPENP2P_BUILD_VERSION=${OPENP2P_BUILD_VERSION}")

# Enable/disable profiling information.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# --- Subdirectories

# All headers are in the /include directory.
include_directories (
	"${PROJECT_SOURCE_DIR}/include"
	)

# Documentation.
add_subdirectory(docs)

# Examples.
add_subdirectory(example)

# Library code.
add_subdirectory(src)

# Tests.
add_subdirectory(tests)

# Tools.
add_subdirectory(tools)

