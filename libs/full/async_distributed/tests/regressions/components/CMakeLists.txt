# Copyright (c) 2007-2014 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_hpx_component(
  action_move_semantics INTERNAL_FLAGS
  FOLDER "Tests/Regressions/Modules/Full/AsyncDistributed/Actions"
  EXCLUDE_FROM_ALL AUTOGLOB
)

add_hpx_pseudo_target(
  tests.regressions.modules.async_distributed.action_move_semantics
)

add_hpx_pseudo_dependencies(
  tests.regressions.modules.async_distributed
  tests.regressions.modules.async_distributed.action_move_semantics
)

add_hpx_pseudo_dependencies(
  tests.regressions.modules.async_distributed.action_move_semantics
  action_move_semantics_component
)
