//  Copyright (c) 2007-2021 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/async_colocated/async_colocated_fwd.hpp>
#include <hpx/components_base/component_type.hpp>
#include <hpx/futures/future.hpp>
#include <hpx/ini/ini.hpp>
#include <hpx/modules/errors.hpp>
#include <hpx/naming_base/id_type.hpp>
#include <hpx/runtime_distributed/server/runtime_support.hpp>
#include <hpx/serialization/vector.hpp>
#include <hpx/type_support/decay.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace hpx { namespace components { namespace stubs {

    ///////////////////////////////////////////////////////////////////////////
    // The \a runtime_support class is the client side representation of a
    // \a server#runtime_support component
    struct HPX_EXPORT runtime_support
    {
        ///////////////////////////////////////////////////////////////////////
        /// Create a new component \a type using the runtime_support with the
        /// given \a targetgid. This is a non-blocking call. The caller needs
        /// to call \a future#get on the result of this function
        /// to obtain the global id of the newly created object.
        template <typename Component, typename... Ts>
        static hpx::future<naming::id_type> create_component_async(
            naming::id_type const& gid, Ts&&... vs)
        {
            if (!naming::is_locality(gid))
            {
                HPX_THROW_EXCEPTION(bad_parameter,
                    "stubs::runtime_support::create_component_async",
                    "The id passed as the first argument is not representing"
                    " a locality");
                return hpx::make_ready_future(naming::invalid_id);
            }

            typedef server::create_component_action<Component,
                typename std::decay<Ts>::type...>
                action_type;
            return hpx::async<action_type>(gid, HPX_FORWARD(Ts, vs)...);
        }

        /// Create a new component \a type using the runtime_support with the
        /// given \a targetgid. Block for the creation to finish.
        template <typename Component, typename... Ts>
        static naming::id_type create_component(
            naming::id_type const& gid, Ts&&... vs)
        {
            return create_component_async<Component>(
                gid, HPX_FORWARD(Ts, vs)...)
                .get();
        }

        /// Create multiple new components \a type using the runtime_support
        /// colocated with the with the given \a targetgid. This is a
        /// non-blocking call.
        template <typename Component, typename... Ts>
        static hpx::future<std::vector<naming::id_type>>
        bulk_create_component_colocated_async(
            naming::id_type const& gid, std::size_t count, Ts&&... vs)
        {
            typedef server::bulk_create_component_action<Component,
                typename std::decay<Ts>::type...>
                action_type;

            return hpx::detail::async_colocated<action_type>(
                gid, count, HPX_FORWARD(Ts, vs)...);
        }

        /// Create multiple new components \a type using the runtime_support
        /// colocated with the with the given \a targetgid. Block for the
        /// creation to finish.
        template <typename Component, typename... Ts>
        static std::vector<naming::id_type> bulk_create_component_colocated(
            naming::id_type const& gid, std::size_t count, Ts&&... vs)
        {
            return bulk_create_component_colocated_async<Component>(
                gid, count, HPX_FORWARD(Ts, vs)...)
                .get();
        }

        /// Create multiple new components \a type using the runtime_support
        /// on the given locality. This is a  non-blocking call.
        template <typename Component, typename... Ts>
        static hpx::future<std::vector<naming::id_type>>
        bulk_create_component_async(
            naming::id_type const& gid, std::size_t count, Ts&&... vs)
        {
            if (!naming::is_locality(gid))
            {
                HPX_THROW_EXCEPTION(bad_parameter,
                    "stubs::runtime_support::bulk_create_component_async",
                    "The id passed as the first argument is not representing"
                    " a locality");
                return hpx::make_ready_future(std::vector<naming::id_type>());
            }

            typedef server::bulk_create_component_action<Component,
                typename std::decay<Ts>::type...>
                action_type;
            return hpx::async<action_type>(gid, count, HPX_FORWARD(Ts, vs)...);
        }

        /// Create multiple new components \a type using the runtime_support
        /// on the given locality. Block for the creation to finish.
        template <typename Component, typename... Ts>
        static std::vector<naming::id_type> bulk_create_component(
            naming::id_type const& gid, std::size_t count, Ts&&... vs)
        {
            return bulk_create_component_async<Component>(
                gid, count, HPX_FORWARD(Ts, vs)...)
                .get();
        }

        /// Create a new component \a type using the runtime_support with the
        /// given \a targetgid. This is a non-blocking call. The caller needs
        /// to call \a future#get on the result of this function
        /// to obtain the global id of the newly created object.
        template <typename Component, typename... Ts>
        static hpx::future<naming::id_type> create_component_colocated_async(
            naming::id_type const& gid, Ts&&... vs)
        {
            typedef server::create_component_action<Component,
                typename std::decay<Ts>::type...>
                action_type;
            return hpx::detail::async_colocated<action_type>(
                gid, HPX_FORWARD(Ts, vs)...);
        }

        /// Create a new component \a type using the runtime_support with the
        /// given \a targetgid. Block for the creation to finish.
        template <typename Component, typename... Ts>
        static naming::id_type create_component_colocated(
            naming::id_type const& gid, Ts&&... vs)
        {
            return create_component_colocated_async<Component>(
                gid, HPX_FORWARD(Ts, vs)...)
                .get();
        }

        ///////////////////////////////////////////////////////////////////////
        // copy construct a component
        template <typename Component>
        static hpx::future<naming::id_type> copy_create_component_async(
            naming::id_type const& gid, std::shared_ptr<Component> const& p,
            bool local_op)
        {
            if (!naming::is_locality(gid))
            {
                HPX_THROW_EXCEPTION(bad_parameter,
                    "stubs::runtime_support::copy_create_component_async",
                    "The id passed as the first argument is not representing"
                    " a locality");
                return hpx::make_ready_future(naming::invalid_id);
            }

            typedef typename server::copy_create_component_action<Component>
                action_type;
            return hpx::async<action_type>(gid, p, local_op);
        }

        template <typename Component>
        static naming::id_type copy_create_component(naming::id_type const& gid,
            std::shared_ptr<Component> const& p, bool local_op)
        {
            // The following get yields control while the action above
            // is executed and the result is returned to the future
            return copy_create_component_async<Component>(gid, p, local_op)
                .get();
        }

        ///////////////////////////////////////////////////////////////////////
        // copy construct a component
        template <typename Component>
        static hpx::future<naming::id_type> migrate_component_async(
            naming::id_type const& target_locality,
            std::shared_ptr<Component> const& p,
            naming::id_type const& to_migrate)
        {
            if (!naming::is_locality(target_locality))
            {
                HPX_THROW_EXCEPTION(bad_parameter,
                    "stubs::runtime_support::migrate_component_async",
                    "The id passed as the first argument is not representing"
                    " a locality");
                return hpx::make_ready_future(naming::invalid_id);
            }

            typedef typename server::migrate_component_here_action<Component>
                action_type;
            return hpx::async<action_type>(target_locality, p, to_migrate);
        }

        template <typename Component, typename DistPolicy>
        static hpx::future<naming::id_type> migrate_component_async(
            DistPolicy const& policy, std::shared_ptr<Component> const& p,
            naming::id_type const& to_migrate)
        {
            typedef typename server::migrate_component_here_action<Component>
                action_type;
            return hpx::async<action_type>(policy, p, to_migrate);
        }

        template <typename Component, typename Target>
        static naming::id_type migrate_component(Target const& target,
            naming::id_type const& to_migrate,
            std::shared_ptr<Component> const& p)
        {
            // The following get yields control while the action above
            // is executed and the result is returned to the future
            return migrate_component_async<Component>(target, p, to_migrate)
                .get();
        }

        ///////////////////////////////////////////////////////////////////////
        static hpx::future<int> load_components_async(
            naming::id_type const& gid);
        static int load_components(naming::id_type const& gid);

        static hpx::future<void> call_startup_functions_async(
            naming::id_type const& gid, bool pre_startup);
        static void call_startup_functions(
            naming::id_type const& gid, bool pre_startup);

        /// \brief Shutdown the given runtime system
        static hpx::future<void> shutdown_async(
            naming::id_type const& targetgid, double timeout = -1);
        static void shutdown(
            naming::id_type const& targetgid, double timeout = -1);

        /// \brief Shutdown the runtime systems of all localities
        static void shutdown_all(
            naming::id_type const& targetgid, double timeout = -1);

        static void shutdown_all(double timeout = -1);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Retrieve configuration information
        /// \brief Terminate the given runtime system
        static hpx::future<void> terminate_async(
            naming::id_type const& targetgid);

        static void terminate(naming::id_type const& targetgid);

        /// \brief Terminate the runtime systems of all localities
        static void terminate_all(naming::id_type const& targetgid);

        static void terminate_all();

        ///////////////////////////////////////////////////////////////////////
        static void garbage_collect_non_blocking(
            naming::id_type const& targetgid);

        static hpx::future<void> garbage_collect_async(
            naming::id_type const& targetgid);

        static void garbage_collect(naming::id_type const& targetgid);

        ///////////////////////////////////////////////////////////////////////
        static hpx::future<naming::id_type> create_performance_counter_async(
            naming::id_type targetgid,
            performance_counters::counter_info const& info);
        static naming::id_type create_performance_counter(
            naming::id_type targetgid,
            performance_counters::counter_info const& info,
            error_code& ec = throws);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Retrieve configuration information
        static hpx::future<util::section> get_config_async(
            naming::id_type const& targetgid);
        static void get_config(
            naming::id_type const& targetgid, util::section& ini);

        ///////////////////////////////////////////////////////////////////////
        static void remove_from_connection_cache_async(
            naming::id_type const& target, naming::gid_type const& gid,
            parcelset::endpoints_type const& endpoints);
    };
}}}    // namespace hpx::components::stubs
