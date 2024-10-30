# ########################################################################
# Copyright 2019 Advanced Micro Devices, Inc.
# ########################################################################

# rocThrust header-only library

# Include the necessary modules
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/ROCMExportTargetsHeaderOnly.cmake)

# Configure a header file to pass the rocThrust version
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/rocthrust_version.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/rocthrust_version.hpp"
  @ONLY
)

# Create wrapper for generated version file
if(BUILD_FILE_REORG_BACKWARD_COMPATIBILITY AND NOT WIN32)
  rocm_wrap_header_file(
      "rocthrust_version.hpp"
      HEADER_LOCATION include/thrust
      WRAPPER_LOCATIONS rocthrust/${CMAKE_INSTALL_INCLUDEDIR}/thrust
      OUTPUT_LOCATIONS rocthrust/wrapper/include/thrust
  )
endif()

# Only header target, does not include dependencies
add_library(rocthrust INTERFACE)

# Common include directories
target_include_directories(rocthrust
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

if(NOT DEFINED CHIP_BUILD_ROCTHRUST)
  # Standalone ROCm build
  target_include_directories(rocthrust
    INTERFACE
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/>
  )

  target_link_libraries(rocthrust
    INTERFACE
      roc::rocprim_hip # Use ROCm package target
  )

  # Installation
  rocm_install_targets(
    TARGETS rocthrust
  )

  if(CMAKE_VERSION VERSION_LESS 3.7)
    set(EXCLUDE_PATTERNS PATTERN "./system/cuda/detail/cub/*" EXCLUDE)
  endif()

  rocm_install(
    DIRECTORY
      "./"
      "${PROJECT_BINARY_DIR}/thrust/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/thrust
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.cuh"
    PATTERN "*.hpp"
    PATTERN "*.in"
    PATTERN "*.inl"
    ${EXCLUDE_PATTERNS}
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
  )

  # Install the wrapper to rocthrust folder
  if (BUILD_FILE_REORG_BACKWARD_COMPATIBILITY AND NOT WIN32)
    rocm_install(
      DIRECTORY
      "${PROJECT_BINARY_DIR}/rocthrust/wrapper/"
      DESTINATION rocthrust/
    )
  endif()

  # Export targets
  rocm_export_targets_header_only(
    TARGETS roc::rocthrust
    DEPENDS PACKAGE rocprim
    NAMESPACE roc::
  )
else()
  # chipStar integrated build
  # Link against the rocprim target built by the rocPRIM subdirectory
  target_link_libraries(rocthrust
    INTERFACE
      rocprim # Use chipStar's integrated target
  )
endif()

