# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Default location is $HPX_ROOT/libs/hardware/include
set(hardware_headers
    hpx/hardware/timestamp/bgq.hpp
    hpx/hardware/timestamp/cuda.hpp
    hpx/hardware/timestamp/linux_generic.hpp
    hpx/hardware/timestamp/linux_x86_32.hpp
    hpx/hardware/timestamp/linux_x86_64.hpp
    hpx/hardware/timestamp/msvc.hpp
    hpx/hardware/timestamp.hpp
)

# Default location is $HPX_ROOT/libs/hardware/include_compatibility
# cmake-format: off
set(hardware_compat_headers
    hpx/util/hardware/timestamp/bgq.hpp => hpx/modules/hardware.hpp
    hpx/util/hardware/timestamp/linux_generic.hpp => hpx/modules/hardware.hpp
    hpx/util/hardware/timestamp/linux_x86_32.hpp => hpx/modules/hardware.hpp
    hpx/util/hardware/timestamp/linux_x86_64.hpp => hpx/modules/hardware.hpp
    hpx/util/hardware/timestamp/msvc.hpp => hpx/modules/hardware.hpp
    hpx/util/hardware/timestamp.hpp => hpx/modules/hardware.hpp
)
# cmake-format: on

# Default location is $HPX_ROOT/libs/hardware/src
set(hardware_sources)

include(HPX_AddModule)

add_hpx_module(
  core hardware
  EXCLUDE_FROM_GLOBAL_HEADER
    "hpx/hardware/timestamp/bgq.hpp"
    "hpx/hardware/timestamp/cuda.hpp"
    "hpx/hardware/timestamp/linux_generic.hpp"
    "hpx/hardware/timestamp/linux_x86_32.hpp"
    "hpx/hardware/timestamp/linux_x86_64.hpp"
    "hpx/hardware/timestamp/msvc.hpp"
  SOURCES ${hardware_sources}
  HEADERS ${hardware_headers}
  COMPAT_HEADERS ${hardware_compat_headers}
  MODULE_DEPENDENCIES hpx_config
  CMAKE_SUBDIRS examples tests
)
