//  Copyright (c) 2019 Ste||ar Group
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/coroutines/config/defines.hpp>
#include <hpx/coroutines/thread_enums.hpp>

#if defined(HPX_COROUTINES_HAVE_DEPRECATION_WARNINGS)
#if defined(HPX_MSVC)
#pragma message(                                                               \
    "The header hpx/runtime/threads/thread_enums.hpp is deprecated, \
    please include hpx/coroutines/thread_enums.hpp instead")
#else
#warning "The header hpx/runtime/threads/thread_enums.hpp is deprecated, \
    please include hpx/coroutines/thread_enums.hpp instead"
#endif
#endif
