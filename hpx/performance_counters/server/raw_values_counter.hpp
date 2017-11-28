//  Copyright (c) 2007-2016 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PERFORMANCE_COUNTERS_SERVER_RAW_VALUES_COUNTER_JUL_16_2016_1019AM)
#define HPX_PERFORMANCE_COUNTERS_SERVER_RAW_VALUES_COUNTER_JUL_16_2016_1019AM

#include <hpx/config.hpp>
#include <hpx/performance_counters/server/base_performance_counter.hpp>
#include <hpx/runtime/components/server/component_base.hpp>
#include <hpx/util/function.hpp>

#include <cstdint>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace performance_counters { namespace server
{
    class HPX_EXPORT raw_values_counter
      : public base_performance_counter,
        public components::component_base<raw_values_counter>
    {
        typedef components::component_base<raw_values_counter> base_type;

    public:
        typedef raw_values_counter type_holder;
        typedef base_performance_counter base_type_holder;

        raw_values_counter()
          : reset_(false)
        {}

        raw_values_counter(counter_info const& info,
            hpx::util::function_nonser<std::vector<std::int64_t>(bool)> f);

        hpx::performance_counters::counter_values_array
            get_counter_values_array(bool reset = false);
        void reset_counter_value();

        /// \brief finalize() will be called just before the instance gets
        ///        destructed
        void finalize()
        {
            base_performance_counter::finalize();
            base_type::finalize();
        }

    private:
        hpx::util::function_nonser<std::vector<std::int64_t>(bool)> f_;
        bool reset_;
    };
}}}

#endif
