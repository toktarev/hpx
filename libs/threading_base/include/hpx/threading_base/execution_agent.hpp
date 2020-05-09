//  Copyright (c) 2019 Thomas Heller
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#include <hpx/basic_execution/agent_base.hpp>
#include <hpx/basic_execution/context_base.hpp>
#include <hpx/basic_execution/resource_base.hpp>
#include <hpx/coroutines/detail/coroutine_impl.hpp>
#include <hpx/coroutines/detail/coroutine_stackful_self.hpp>
#include <hpx/coroutines/thread_enums.hpp>
#include <hpx/coroutines/thread_id_type.hpp>
#include <hpx/timing/steady_clock.hpp>

#include <cstddef>
#include <string>

#include <hpx/config/warnings_prefix.hpp>

namespace hpx { namespace threads {

    struct HPX_EXPORT execution_context : hpx::basic_execution::context_base
    {
        hpx::basic_execution::resource_base const& resource() const override
        {
            return resource_;
        }
        hpx::basic_execution::resource_base resource_;
    };

    struct HPX_EXPORT execution_agent : hpx::basic_execution::agent_base
    {
        explicit execution_agent(
            coroutines::detail::coroutine_impl* coroutine) noexcept;

        std::string description() const override;

        execution_context const& context() const override
        {
            return context_;
        }

        void yield(char const* desc) override;
        void yield_k(std::size_t k, char const* desc) override;
        void suspend(char const* desc) override;
        void resume(char const* desc) override;
        void abort(char const* desc) override;
        void sleep_for(hpx::util::steady_duration const& sleep_duration,
            char const* desc) override;
        void sleep_until(hpx::util::steady_time_point const& sleep_time,
            char const* desc) override;

    private:
        coroutines::detail::coroutine_stackful_self self_;

        hpx::threads::thread_state_ex_enum do_yield(
            char const* desc, threads::thread_state_enum state);

        void do_resume(
            char const* desc, hpx::threads::thread_state_ex_enum statex);

        execution_context context_;
    };
}}    // namespace hpx::threads

#include <hpx/config/warnings_suffix.hpp>
