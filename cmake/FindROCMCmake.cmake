# ########################################################################
# Copyright 2021 Advanced Micro Devices, Inc.
# ########################################################################

# ###########################
# rocThrust dependencies
# ###########################

if(NOT ROCM_PATH)
  set(ROCM_PATH /opt/rocm)
endif()

find_package(ROCM CONFIG QUIET PATHS ${ROCM_PATH} /opt/rocm)
if(NOT ROCM_FOUND)
  include(FetchContent)
  set(rocm_cmake_tag "master" CACHE STRING "rocm-cmake tag to download")
  FetchContent_Declare(
    rocm-cmake
    URL https://github.com/RadeonOpenCompute/rocm-cmake/archive/${rocm_cmake_tag}.tar.gz
  )
  FetchContent_GetProperties(rocm-cmake)
  if(NOT rocm-cmake_POPULATED)
    FetchContent_Populate(rocm-cmake)
  endif()
  find_package(ROCM CONFIG REQUIRED NO_DEFAULT_PATH HINTS "${rocm-cmake_SOURCE_DIR}")
endif()

