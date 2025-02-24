//  Copyright (c) 2007-2011 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Adelstein-Lelbach
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/assert.hpp>
#include <hpx/include/components.hpp>

#include <string>

#include "stubs/simple_central_tuplespace.hpp"

namespace examples
{
    ///////////////////////////////////////////////////////////////////////////
    /// Client for the \a server::simple_central_tuplespace component.
    //[simple_central_tuplespace_client_inherit
    class simple_central_tuplespace
      : public hpx::components::client_base<
            simple_central_tuplespace, stubs::simple_central_tuplespace
        >
    //]
    {
        //[simple_central_tuplespace_base_type
        typedef hpx::components::client_base<
            simple_central_tuplespace, stubs::simple_central_tuplespace
        > base_type;
        //]

        typedef base_type::tuple_type tuple_type;

    public:
        /// Default construct an empty client side representation (not
        /// connected to any existing component).
        simple_central_tuplespace()
        {}

        /// Create a client side representation for the existing
        /// \a server::simple_central_tuplespace instance with the given GID.
        simple_central_tuplespace(hpx::shared_future<hpx::naming::id_type> const& gid)
          : base_type(gid)
        {}

        simple_central_tuplespace(hpx::naming::id_type const& gid)
          : base_type(gid)
        {}

        ~simple_central_tuplespace()
        {
            if (!symbol_name_.empty())
            {
                hpx::agas::unregister_name(symbol_name_);
            }
        }

        bool create(std::string const& symbol_name, hpx::id_type const& locality)
        {
            if(!symbol_name_.empty())
            {
                hpx::cerr<<"simple_central_tuplespace::create() "
                    <<": ERROR! current instance not empty!\n";
                return false;
            }
            if(symbol_name_ == symbol_name) // itself
            {
                hpx::cerr<<"simple_central_tuplespace::create() "
                    <<": ERROR! current instance already attached to "
                    << symbol_name <<"\n";
                return false;
            }

            // request gid;
            *this = hpx::components::new_<simple_central_tuplespace>(locality);
            bool rc = hpx::agas::register_name(hpx::launch::sync, symbol_name,
                this->get_id());

            if(rc)
            {
                symbol_name_ = symbol_name;
            }

            return rc;
        }

        bool connect(std::string const& symbol_name)
        {
            if(symbol_name_ == symbol_name)
            {
                hpx::cerr<<"simple_central_tuplespace::connect()"
                    <<" : ERROR! current instance already attached to "
                    << symbol_name <<"\n";
                return false;
            }

            *this = hpx::agas::resolve_name(hpx::launch::sync,symbol_name);

            return true;
        }

        ///////////////////////////////////////////////////////////////////////
        /// put \p tuple into tuplespace.
        ///
        /// \note This function has fire-and-forget semantics. It will not wait
        ///       for the action to be executed. Instead, it will return
        ///       immediately after the action has has been dispatched.
        //[simple_central_tuplespace_client_write_async
        hpx::future<int> write_async(tuple_type const& tuple)
        {
            HPX_ASSERT(this->get_id());
            return this->base_type::write_async(this->get_id(), tuple);
        }
        //]

        /// put \p tuple into tuplespace.
        ///
        /// \note This function is fully synchronous.
        int write(hpx::launch::sync_policy, tuple_type const& tuple)
        {
            HPX_ASSERT(this->get_id());
            return this->base_type::write(hpx::launch::sync, this->get_id(), tuple);
        }

        ///////////////////////////////////////////////////////////////////////
        /// read matching tuple from tuplespace within \p timeout.
        ///
        /// \note This function has fire-and-forget semantics. It will not wait
        ///       for the action to be executed. Instead, it will return
        ///       immediately after the action has has been dispatched.
        hpx::future<tuple_type>
            read_async(tuple_type const& tp, double const timeout)
        {
            HPX_ASSERT(this->get_id());
            return this->base_type::read_async(this->get_id(), tp, timeout);
        }

        /// read matching tuple from tuplespace within \p timeout.
        ///
        /// \note This function is fully synchronous.
        //[simple_central_tuplespace_client_read_sync
        tuple_type read(hpx::launch::sync_policy, tuple_type const& tp,
            double const timeout)
        {
            HPX_ASSERT(this->get_id());
            return this->base_type::read(hpx::launch::sync, this->get_id(),
                tp, timeout);
        }
        //]

        ///////////////////////////////////////////////////////////////////////
        /// take matching tuple from tuplespace within \p timeout.
        ///
        /// \returns This function returns an \a hpx::future. When the
        ///          value of this computation is needed, the get() method of
        ///          the future should be called. If the value is available,
        ///          get() will return immediately; otherwise, it will block
        ///          until the value is ready.
        //[simple_central_tuplespace_client_take_async
        hpx::future<tuple_type>
            take_async(tuple_type const& tp, double const timeout)
        {
            HPX_ASSERT(this->get_id());
            return this->base_type::take(hpx::launch::async, this->get_id(),
                tp, timeout);
        }
        //]

        /// take matching tuple from tuplespace within \p timeout.
        ///
        /// \note This function is fully synchronous.
        tuple_type take(hpx::launch::sync_policy, tuple_type const& tp,
            double const timeout)
        {
            HPX_ASSERT(this->get_id());
            return this->base_type::take(hpx::launch::sync, this->get_id(),
                tp, timeout);
        }

    private:
        std::string symbol_name_;
    };
} // examples


