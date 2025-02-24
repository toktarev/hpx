//  Copyright (c) 2007-2021 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#include <hpx/actions_base/basic_action.hpp>
#include <hpx/assert.hpp>
#include <hpx/async_distributed/applier/apply.hpp>
#include <hpx/async_distributed/continuation.hpp>
#include <hpx/async_distributed/transfer_continuation_action.hpp>
#include <hpx/components_base/agas_interface.hpp>
#include <hpx/functional/bind_front.hpp>
#include <hpx/functional/function.hpp>
#include <hpx/futures/packaged_continuation.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/modules/format.hpp>
#include <hpx/performance_counters/base_performance_counter.hpp>
#include <hpx/performance_counters/counter_creators.hpp>
#include <hpx/performance_counters/counter_interface.hpp>
#include <hpx/performance_counters/counter_parser.hpp>
#include <hpx/performance_counters/counters.hpp>
#include <hpx/performance_counters/manage_counter_type.hpp>
#include <hpx/performance_counters/registry.hpp>
#include <hpx/runtime_local/get_num_all_localities.hpp>
#include <hpx/runtime_local/get_os_thread_count.hpp>
#include <hpx/runtime_local/runtime_local_fwd.hpp>
#include <hpx/runtime_local/thread_pool_helpers.hpp>
#include <hpx/serialization/base_object.hpp>
#include <hpx/serialization/serialize.hpp>
#include <hpx/serialization/string.hpp>
#include <hpx/serialization/vector.hpp>
#include <hpx/threading_base/thread_helpers.hpp>
#include <hpx/util/from_string.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Initialization support for the performance counter actions
HPX_REGISTER_ACTION_ID(hpx::performance_counters::server ::
                           base_performance_counter::get_counter_info_action,
    performance_counter_get_counter_info_action,
    hpx::actions::performance_counter_get_counter_info_action_id)
HPX_REGISTER_ACTION_ID(hpx::performance_counters::server ::
                           base_performance_counter::get_counter_value_action,
    performance_counter_get_counter_value_action,
    hpx::actions::performance_counter_get_counter_value_action_id)
HPX_REGISTER_ACTION_ID(
    hpx::performance_counters::server ::base_performance_counter::
        get_counter_values_array_action,
    performance_counter_get_counter_values_array_action,
    hpx::actions::performance_counter_get_counter_values_array_action_id)
HPX_REGISTER_ACTION_ID(hpx::performance_counters::server ::
                           base_performance_counter::set_counter_value_action,
    performance_counter_set_counter_value_action,
    hpx::actions::performance_counter_set_counter_value_action_id)
HPX_REGISTER_ACTION_ID(hpx::performance_counters::server ::
                           base_performance_counter::reset_counter_value_action,
    performance_counter_reset_counter_value_action,
    hpx::actions::performance_counter_reset_counter_value_action_id)
HPX_REGISTER_ACTION_ID(
    hpx::performance_counters::server::base_performance_counter::start_action,
    performance_counter_start_action,
    hpx::actions::performance_counter_start_action_id)
HPX_REGISTER_ACTION_ID(
    hpx::performance_counters::server::base_performance_counter::stop_action,
    performance_counter_stop_action,
    hpx::actions::performance_counter_stop_action_id)

HPX_REGISTER_BASE_LCO_WITH_VALUE_ID(hpx::performance_counters::counter_info,
    hpx_counter_info, hpx::actions::base_lco_with_value_hpx_counter_info_get,
    hpx::actions::base_lco_with_value_hpx_counter_info_set)
HPX_REGISTER_BASE_LCO_WITH_VALUE_ID(hpx::performance_counters::counter_value,
    hpx_counter_value, hpx::actions::base_lco_with_value_hpx_counter_value_get,
    hpx::actions::base_lco_with_value_hpx_counter_value_set)
HPX_REGISTER_BASE_LCO_WITH_VALUE_ID(
    hpx::performance_counters::counter_values_array, hpx_counter_values_array,
    hpx::actions::base_lco_with_value_hpx_counter_values_array_get,
    hpx::actions::base_lco_with_value_hpx_counter_values_array_set)

HPX_DEFINE_GET_COMPONENT_TYPE(
    hpx::performance_counters::server::base_performance_counter)

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace performance_counters {
    /// \brief Create a full name of a counter from the contents of the given
    ///        \a counter_path_elements instance.
    counter_status get_counter_name(
        counter_path_elements const& path, std::string& result, error_code& ec)
    {
        if (path.objectname_.empty())
        {
            HPX_THROWS_IF(ec, bad_parameter, "get_counter_name",
                "empty counter object name");
            return status_invalid_data;
        }

        result = "/";
        result += path.objectname_;

        if (!path.parentinstancename_.empty() || !path.instancename_.empty() ||
            !path.subinstancename_.empty())
        {
            result += "{";
            if (!path.parentinstancename_.empty())
            {
                result += path.parentinstancename_;
                if (-1 != path.parentinstanceindex_)
                {
                    result += "#";
                    result += std::to_string(path.parentinstanceindex_);
                }
            }
            if (!path.instancename_.empty())
            {
                result += "/";
                result += path.instancename_;
                if (path.instanceindex_ != -1)
                {
                    if (path.instancename_ == "pool")
                    {
                        result += "#" +
                            hpx::resource::get_pool_name(path.instanceindex_);
                    }
                    else
                    {
                        result += "#" + std::to_string(path.instanceindex_);
                    }
                }
            }
            if (!path.subinstancename_.empty())
            {
                result += "/";
                result += path.subinstancename_;
                if (-1 != path.subinstanceindex_)
                {
                    result += "#";
                    result += std::to_string(path.subinstanceindex_);
                }
            }
            result += "}";
        }
        if (!path.countername_.empty())
        {
            result += "/";
            result += path.countername_;
        }

        if (!path.parameters_.empty())
        {
            result += "@";
            result += path.parameters_;
        }

        if (&ec != &throws)
            ec = make_success_code();
        return status_valid_data;
    }

    /// \brief Create a full name of a counter from the contents of the given
    ///        \a counter_path_elements instance.
    counter_status get_counter_type_name(counter_type_path_elements const& path,
        std::string& result, error_code& ec)
    {
        if (path.objectname_.empty())
        {
            HPX_THROWS_IF(ec, bad_parameter, "get_counter_type_name",
                "empty counter object name");
            return status_invalid_data;
        }

        result = "/";
        result += path.objectname_;

        if (!path.countername_.empty())
        {
            result += "/";
            result += path.countername_;
        }

        if (&ec != &throws)
            ec = make_success_code();
        return status_valid_data;
    }

    /// \brief Create a full name of a counter from the contents of the given
    ///        \a counter_path_elements instance.
    counter_status get_full_counter_type_name(
        counter_type_path_elements const& path, std::string& result,
        error_code& ec)
    {
        if (path.objectname_.empty())
        {
            HPX_THROWS_IF(ec, bad_parameter, "get_full_counter_type_name",
                "empty counter object name");
            return status_invalid_data;
        }

        result = "/";
        result += path.objectname_;

        if (!path.countername_.empty())
        {
            result += "/";
            result += path.countername_;
        }

        if (!path.parameters_.empty())
        {
            result += "@";
            result += path.parameters_;
        }

        if (&ec != &throws)
            ec = make_success_code();
        return status_valid_data;
    }

    /// \brief Create a name of a counter instance from the contents of the
    ///        given \a counter_path_elements instance.
    counter_status get_counter_instance_name(
        counter_path_elements const& path, std::string& result, error_code& ec)
    {
        if (path.parentinstancename_.empty())
        {
            HPX_THROWS_IF(ec, bad_parameter, "get_counter_instance_name",
                "empty counter instance name");
            return status_invalid_data;
        }

        if (path.parentinstance_is_basename_)
        {
            result = path.parentinstancename_;
        }
        else
        {
            result = "/";
            result += path.parentinstancename_;
            if (path.parentinstanceindex_ == -1)
            {
                result += "#*";
            }
            else
            {
                result += "#" + std::to_string(path.parentinstanceindex_);
            }

            if (!path.instancename_.empty())
            {
                result += "/";
                result += path.instancename_;
                if (path.instanceindex_ == -1)
                {
                    result += "#*";
                }
                else if (path.instancename_ == "pool")
                {
                    result +=
                        "#" + hpx::resource::get_pool_name(path.instanceindex_);
                }
                else
                {
                    result += "#" + std::to_string(path.instanceindex_);
                }
            }

            if (!path.subinstancename_.empty())
            {
                result += "/";
                result += path.subinstancename_;
                if (path.subinstanceindex_ == -1)
                {
                    result += "#*";
                }
                else
                {
                    result += "#" + std::to_string(path.subinstanceindex_);
                }
            }
        }

        if (&ec != &throws)
            ec = make_success_code();
        return status_valid_data;
    }

    /// \brief Fill the given \a counter_path_elements instance from the given
    ///        full name of a counter
    ///
    ///    /objectname{parentinstancename#parentindex
    ///       /instancename#instanceindex}/countername
    ///
    counter_status get_counter_path_elements(
        std::string const& name, counter_path_elements& path, error_code& ec)
    {
        path_elements elements;
        if (!parse_counter_name(name, elements))
        {
            HPX_THROWS_IF(ec, bad_parameter, "get_counter_path_elements",
                "invalid counter name format: {}", name);
            return status_invalid_data;
        }

        path.objectname_ = elements.object_;
        path.countername_ = elements.counter_;

        path.parentinstancename_ = elements.instance_.parent_.name_;
        path.parentinstance_is_basename_ = elements.instance_.parent_.basename_;
        path.parentinstanceindex_ = -1;

        path.instancename_ = elements.instance_.child_.name_;
        path.instanceindex_ = -1;

        path.subinstancename_ = elements.instance_.subchild_.name_;
        path.subinstanceindex_ = -1;

        path.parameters_ = elements.parameters_;

        if (!path.parentinstance_is_basename_)
        {
            if (elements.instance_.parent_.index_ == "#*")
            {
                path.parentinstancename_ += "#*";
            }
            else if (!elements.instance_.parent_.index_.empty())
            {
                path.parentinstanceindex_ =
                    hpx::util::from_string<std::int64_t>(
                        elements.instance_.parent_.index_);
            }

            if (elements.instance_.child_.index_ == "#*")
            {
                path.instancename_ += "#*";
            }
            else if (!elements.instance_.child_.index_.empty())
            {
                if (elements.instance_.child_.index_[0] == '#')
                    elements.instance_.child_.index_.erase(0, 1);

                if (path.instancename_ == "pool")
                {
                    path.instanceindex_ = hpx::resource::get_pool_index(
                        elements.instance_.child_.index_);
                }
                else
                {
                    path.instanceindex_ = hpx::util::from_string<std::int64_t>(
                        elements.instance_.child_.index_, std::int64_t(-2));
                    if (path.instanceindex_ == std::int64_t(-2))
                    {
                        HPX_THROWS_IF(ec, bad_parameter,
                            "get_counter_path_elements",
                            "invalid counter name format: {}", name);
                        return status_invalid_data;
                    }
                }
            }

            if (elements.instance_.subchild_.index_ == "#*")
            {
                path.subinstancename_ += "#*";
            }
            else if (!elements.instance_.subchild_.index_.empty())
            {
                path.subinstanceindex_ = hpx::util::from_string<std::uint64_t>(
                    elements.instance_.subchild_.index_);
            }
        }

        if (&ec != &throws)
            ec = make_success_code();

        return status_valid_data;
    }

    /// \brief Fill the given \a counter_type_path_elements instance from the
    ///        given full name of a counter
    ///
    ///    /objectname{...}/countername
    ///    /objectname
    ///
    counter_status get_counter_type_path_elements(std::string const& name,
        counter_type_path_elements& path, error_code& ec)
    {
        // parse the full name
        path_elements elements;
        if (!parse_counter_name(name, elements))
        {
            HPX_THROWS_IF(ec, bad_parameter, "get_counter_type_path_elements",
                "invalid counter name format: {}", name);
            return status_invalid_data;
        }

        // but extract only counter type elements
        path.objectname_ = elements.object_;
        path.countername_ = elements.counter_;
        path.parameters_ = elements.parameters_;

        if (&ec != &throws)
            ec = make_success_code();

        return status_valid_data;
    }

    /// \brief Return the counter type name from a given full instance name
    counter_status get_counter_type_name(
        std::string const& name, std::string& type_name, error_code& ec)
    {
        counter_type_path_elements p;

        counter_status status = get_counter_type_path_elements(name, p, ec);
        if (!status_is_valid(status))
            return status;

        return get_counter_type_name(p, type_name, ec);
    }

    /// \brief Return the counter type name from a given full instance name
    counter_status get_counter_name(
        std::string const& name, std::string& countername, error_code& ec)
    {
        counter_path_elements p;

        counter_status status = get_counter_path_elements(name, p, ec);
        if (!status_is_valid(status))
            return status;

        return get_counter_name(p, countername, ec);
    }

    /// \brief Complement the counter info if parent instance name is missing
    counter_status complement_counter_info(
        counter_info& info, counter_info const& type_info, error_code& ec)
    {
        info.type_ = type_info.type_;
        if (info.helptext_.empty())
            info.helptext_ = type_info.helptext_;
        return complement_counter_info(info, ec);
    }

    counter_status complement_counter_info(counter_info& info, error_code& ec)
    {
        counter_path_elements p;

        counter_status status =
            get_counter_path_elements(info.fullname_, p, ec);
        if (!status_is_valid(status))
            return status;

        if (p.parentinstancename_.empty())
        {
            p.parentinstancename_ = "locality";
            p.parentinstanceindex_ =
                static_cast<std::int64_t>(get_locality_id());
            if (p.instancename_.empty())
            {
                p.instancename_ = "total";
                p.instanceindex_ = -1;
            }
            if (p.subinstancename_.empty())
            {
                p.subinstanceindex_ = -1;
            }
        }

        // fill with complete counter type info
        std::string type_name;
        get_counter_type_name(p, type_name, ec);
        if (!status_is_valid(status))
            return status;

        get_counter_type(type_name, info, ec);
        if (!status_is_valid(status))
            return status;

        // last, set full counter name
        return get_counter_name(p, info.fullname_, ec);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the name for a given counter type
    namespace strings {
        char const* const counter_type_names[] = {
            "counter_text",
            "counter_raw",
            "counter_monotonically_increasing",
            "counter_average_base",
            "counter_average_count",
            "counter_aggregating",
            "counter_average_timer",
            "counter_elapsed_time",
            "counter_histogram",
            "counter_raw_values",
        };
    }

    char const* get_counter_type_name(counter_type type)
    {
        if (type < counter_text || type > counter_raw_values)
            return "unknown";
        return strings::counter_type_names[type];
    }

    ///////////////////////////////////////////////////////////////////////////
    counter_status add_counter_type(counter_info const& info,
        create_counter_func const& create_counter,
        discover_counters_func const& discover_counters, error_code& ec)
    {
        if (hpx::get_runtime_ptr() == nullptr)
        {
            HPX_THROWS_IF(ec, bad_parameter,
                "performance_counters::add_counter_type",
                "the runtime is not currently running");
            return status_generic_error;
        }
        return registry::instance().add_counter_type(
            info, create_counter, discover_counters, ec);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Call the supplied function for each registered counter type
    counter_status discover_counter_types(
        discover_counter_func const& discover_counter,
        discover_counters_mode mode, error_code& ec)
    {
        if (hpx::get_runtime_ptr() == nullptr)
        {
            HPX_THROWS_IF(ec, bad_parameter,
                "performance_counters::discover_counter_types",
                "the runtime is not currently running");
            return status_generic_error;
        }
        return registry::instance().discover_counter_types(
            discover_counter, mode, ec);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Call the supplied function for the given registered counter type.
    counter_status discover_counter_type(counter_info const& info,
        discover_counter_func const& discover_counter,
        discover_counters_mode mode, error_code& ec)
    {
        if (hpx::get_runtime_ptr() == nullptr)
        {
            HPX_THROWS_IF(ec, bad_parameter,
                "performance_counters::discover_counter_types",
                "the runtime is not currently running");
            return status_generic_error;
        }
        return registry::instance().discover_counter_type(
            info, discover_counter, mode, ec);
    }

    counter_status discover_counter_type(std::string const& name,
        discover_counter_func const& discover_counter,
        discover_counters_mode mode, error_code& ec)
    {
        if (hpx::get_runtime_ptr() == nullptr)
        {
            HPX_THROWS_IF(ec, bad_parameter,
                "performance_counters::discover_counter_types",
                "the runtime is not currently running");
            return status_generic_error;
        }
        return registry::instance().discover_counter_type(
            name, discover_counter, mode, ec);
    }

    ///////////////////////////////////////////////////////////////////////////
    namespace detail {
        bool discover_counters(std::vector<counter_info>& counters,
            counter_info const& info, error_code& /* ec */)
        {
            counters.push_back(info);
            return true;
        }
    }    // namespace detail

    counter_status discover_counter_types(std::vector<counter_info>& counters,
        discover_counters_mode mode, error_code& ec)
    {
        discover_counter_func func(hpx::util::bind_front(
            &detail::discover_counters, std::ref(counters)));

        return discover_counter_types(HPX_MOVE(func), mode, ec);
    }

    counter_status discover_counter_type(std::string const& name,
        std::vector<counter_info>& counters, discover_counters_mode mode,
        error_code& ec)
    {
        discover_counter_func func(hpx::util::bind_front(
            &detail::discover_counters, std::ref(counters)));

        return discover_counter_type(name, HPX_MOVE(func), mode, ec);
    }

    counter_status discover_counter_type(counter_info const& info,
        std::vector<counter_info>& counters, discover_counters_mode mode,
        error_code& ec)
    {
        discover_counter_func func(hpx::util::bind_front(
            &detail::discover_counters, std::ref(counters)));

        return discover_counter_type(info, HPX_MOVE(func), mode, ec);
    }

    ///////////////////////////////////////////////////////////////////////////
    counter_status remove_counter_type(counter_info const& info, error_code& ec)
    {
        // the runtime could be gone already
        if (hpx::get_runtime_ptr() == nullptr)
        {
            return status_generic_error;
        }
        return registry::instance().remove_counter_type(info, ec);
    }

    /// \brief Retrieve the counter type for the given counter name from the
    ///        (local) registry
    counter_status get_counter_type(
        std::string const& name, counter_info& info, error_code& ec)
    {
        if (hpx::get_runtime_ptr() == nullptr)
        {
            HPX_THROWS_IF(ec, bad_parameter,
                "performance_counters::get_counter_type",
                "the runtime is not currently running");
            return status_generic_error;
        }
        return registry::instance().get_counter_type(name, info, ec);
    }

    namespace detail {
        naming::gid_type create_raw_counter_value(counter_info const& info,
            std::int64_t* countervalue, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_raw_counter_value(
                info, countervalue, gid, ec);
            return gid;
        }

        naming::gid_type create_raw_counter(counter_info const& info,
            hpx::function<std::int64_t()> const& f, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_raw_counter(info, f, gid, ec);
            return gid;
        }

        naming::gid_type create_raw_counter(counter_info const& info,
            hpx::function<std::int64_t(bool)> const& f, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_raw_counter(info, f, gid, ec);
            return gid;
        }

        naming::gid_type create_raw_counter(counter_info const& info,
            hpx::function<std::vector<std::int64_t>()> const& f, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_raw_counter(info, f, gid, ec);
            return gid;
        }

        naming::gid_type create_raw_counter(counter_info const& info,
            hpx::function<std::vector<std::int64_t>(bool)> const& f,
            error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_raw_counter(info, f, gid, ec);
            return gid;
        }

        // \brief Create a new performance counter instance based on given
        //        counter info
        naming::gid_type create_counter(
            counter_info const& info, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_counter(info, gid, ec);
            return gid;
        }

        // \brief Create a new aggregating performance counter instance based
        //        on given base counter name and given base time interval
        //        (milliseconds).
        naming::gid_type create_statistics_counter(counter_info const& info,
            std::string const& base_counter_name,
            std::vector<std::size_t> const& parameters, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_statistics_counter(
                info, base_counter_name, parameters, gid, ec);
            return gid;
        }

        // \brief Create a new aggregating performance counter instance based
        //        on given base counter name and given base time interval
        //        (milliseconds).
        naming::gid_type create_arithmetics_counter(counter_info const& info,
            std::vector<std::string> const& base_counter_names, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_arithmetics_counter(
                info, base_counter_names, gid, ec);
            return gid;
        }

        // \brief Create a new aggregating extended performance counter instance
        //        based on given base counter name and given base time interval
        //        (milliseconds).
        naming::gid_type create_arithmetics_counter_extended(
            counter_info const& info,
            std::vector<std::string> const& base_counter_names, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            naming::gid_type gid;
            registry::instance().create_arithmetics_counter_extended(
                info, base_counter_names, gid, ec);
            return gid;
        }

        ///////////////////////////////////////////////////////////////////////
        counter_status add_counter(
            naming::id_type const& id, counter_info const& info, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            return registry::instance().add_counter(id, info, ec);
        }

        counter_status remove_counter(
            counter_info const& info, naming::id_type const& id, error_code& ec)
        {
            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);
            return registry::instance().remove_counter(info, id, ec);
        }

        // create an arbitrary counter on this locality
        naming::gid_type create_counter_local(counter_info const& info)
        {
            // find create function for given counter
            error_code ec;

            HPX_ASSERT(hpx::get_runtime_ptr() != nullptr);

            create_counter_func f;
            registry::instance().get_counter_create_function(info, f, ec);
            if (ec)
            {
                HPX_THROW_EXCEPTION(bad_parameter, "create_counter_local",
                    "no create function for performance counter found: {} ({})",
                    remove_counter_prefix(info.fullname_), ec.get_message());
                return naming::invalid_gid;
            }

            counter_path_elements paths;
            get_counter_path_elements(info.fullname_, paths, ec);
            if (ec)
                return hpx::naming::invalid_gid;

            if (paths.parentinstancename_ == "locality" &&
                paths.parentinstanceindex_ !=
                    static_cast<std::int64_t>(hpx::get_locality_id()))
            {
                HPX_THROW_EXCEPTION(bad_parameter, "create_counter_local",
                    "attempt to create counter on wrong locality ({})",
                    ec.get_message());
                return hpx::naming::invalid_gid;
            }

            // attempt to create the new counter instance
            naming::gid_type gid = f(info, ec);
            if (ec)
            {
                HPX_THROW_EXCEPTION(bad_parameter, "create_counter_local",
                    "couldn't create performance counter: {} ({})",
                    remove_counter_prefix(info.fullname_), ec.get_message());
                return naming::invalid_gid;
            }

            return gid;
        }

        ///////////////////////////////////////////////////////////////////////
        inline bool is_thread_kind(std::string const& pattern)
        {
            std::string::size_type p = pattern.find("-thread#*");
            return p != std::string::npos && p == pattern.size() - 9;
        }

        inline std::string get_thread_kind(std::string const& pattern)
        {
            HPX_ASSERT(is_thread_kind(pattern));
            return pattern.substr(0, pattern.find_last_of('-'));
        }

        ///////////////////////////////////////////////////////////////////////
        inline bool is_pool_kind(std::string const& pattern)
        {
            std::string::size_type p = pattern.find("pool#*");
            return p != std::string::npos;
        }

        inline std::string get_pool_kind(std::string const& pattern)
        {
            HPX_ASSERT(is_pool_kind(pattern));
            return pattern.substr(0, pattern.find_last_of('#'));
        }

        ///////////////////////////////////////////////////////////////////////
        inline bool is_node_kind(std::string const& pattern)
        {
            std::string::size_type p = pattern.find("-node#*");
            return p != std::string::npos && p == pattern.size() - 7;
        }

        inline std::string get_node_kind(std::string const& pattern)
        {
            HPX_ASSERT(is_node_kind(pattern));
            return pattern.substr(0, pattern.find_last_of('-'));
        }

        ///////////////////////////////////////////////////////////////////////
        /// Expand all wild-cards in a counter base name (for aggregate counters)
        bool expand_basecounter(counter_info const& info,
            counter_path_elements& p, discover_counter_func const& f,
            error_code& ec)
        {
            // discover all base names
            std::vector<counter_info> counter_infos;
            counter_status status = discover_counter_type(p.parentinstancename_,
                counter_infos, discover_counters_full, ec);
            if (!status_is_valid(status) || ec)
                return false;

            counter_info i = info;
            for (counter_info& basei : counter_infos)
            {
                p.parentinstancename_ = basei.fullname_;
                counter_status status = get_counter_name(p, i.fullname_, ec);
                if (!status_is_valid(status) || !f(i, ec) || ec)
                    return false;
            }
            return true;
        }

        ///////////////////////////////////////////////////////////////////////
        // expand main counter name
        bool expand_counter_info_pools(bool expand_threads, counter_info& i,
            counter_path_elements& p, discover_counter_func const& f,
            error_code& ec)
        {
            std::size_t num_pools = hpx::resource::get_num_thread_pools();
            for (std::size_t l = 0; l != num_pools; ++l)
            {
                p.instanceindex_ = static_cast<std::int64_t>(l);

                if (expand_threads)
                {
                    std::size_t num_threads =
                        hpx::resource::get_num_threads(p.instanceindex_);
                    for (std::size_t t = 0; t != num_threads; ++t)
                    {
                        p.subinstanceindex_ = static_cast<std::int64_t>(t);

                        counter_status status =
                            get_counter_name(p, i.fullname_, ec);
                        if (!status_is_valid(status) || !f(i, ec) || ec)
                            return false;
                    }
                }
                else
                {
                    counter_status status =
                        get_counter_name(p, i.fullname_, ec);
                    if (!status_is_valid(status) || !f(i, ec) || ec)
                        return false;
                }
            }
            return true;
        }

        bool expand_counter_info_pool_threads(counter_info& i,
            counter_path_elements& p, discover_counter_func const& f,
            error_code& ec)
        {
            std::size_t num_threads =
                hpx::resource::get_num_threads(p.instanceindex_);
            for (std::size_t t = 0; t != num_threads; ++t)
            {
                p.subinstanceindex_ = static_cast<std::int64_t>(t);

                counter_status status = get_counter_name(p, i.fullname_, ec);
                if (!status_is_valid(status) || !f(i, ec) || ec)
                    return false;
            }
            return true;
        }

        bool expand_counter_info_threads(counter_info& i,
            counter_path_elements& p, discover_counter_func const& f,
            error_code& ec)
        {
            std::size_t num_threads = get_os_thread_count();
            for (std::size_t l = 0; l != num_threads; ++l)
            {
                p.instanceindex_ = static_cast<std::int64_t>(l);
                counter_status status = get_counter_name(p, i.fullname_, ec);
                if (!status_is_valid(status) || !f(i, ec) || ec)
                    return false;
            }
            return true;
        }

        bool expand_counter_info_nodes(counter_info& i,
            counter_path_elements& p, discover_counter_func const& f,
            error_code& ec)
        {
            std::size_t num_nodes =
                hpx::threads::get_topology().get_number_of_numa_nodes();
            for (std::size_t l = 0; l != num_nodes; ++l)
            {
                p.instanceindex_ = static_cast<std::int64_t>(l);
                counter_status status = get_counter_name(p, i.fullname_, ec);
                if (!status_is_valid(status) || !f(i, ec) || ec)
                    return false;
            }
            return true;
        }

        bool expand_counter_info_localities(counter_info& i,
            counter_path_elements& p, discover_counter_func const& f,
            error_code& ec)
        {
            bool expand_pools = false;
            bool expand_threads = false;
            bool expand_nodes = false;

            if (is_pool_kind(p.instancename_))
            {
                p.instancename_ = get_pool_kind(p.instancename_);
                expand_pools = true;

                if (is_thread_kind(p.subinstancename_))
                {
                    p.subinstancename_ =
                        get_thread_kind(p.subinstancename_) + "-thread";
                    expand_threads = true;
                }
            }
            else if (is_thread_kind(p.instancename_))
            {
                p.instancename_ = get_thread_kind(p.instancename_) + "-thread";
                expand_threads = true;
            }
            else if (is_node_kind(p.instancename_))
            {
                p.instancename_ = get_node_kind(p.instancename_) + "-node";
                expand_nodes = true;
            }

            std::uint32_t last_locality = get_num_localities(hpx::launch::sync);
            for (std::uint32_t l = 0; l != last_locality; ++l)
            {
                p.parentinstanceindex_ = static_cast<std::int32_t>(l);
                if (expand_pools)
                {
                    if (!detail::expand_counter_info_pools(
                            expand_threads, i, p, f, ec))
                    {
                        return false;
                    }
                }
                else if (expand_threads)
                {
                    if (!detail::expand_counter_info_threads(i, p, f, ec))
                        return false;
                }
                else if (expand_nodes)
                {
                    if (!detail::expand_counter_info_nodes(i, p, f, ec))
                        return false;
                }
                else
                {
                    counter_status status =
                        get_counter_name(p, i.fullname_, ec);
                    if (!status_is_valid(status) || !f(i, ec) || ec)
                        return false;
                }
            }
            return true;
        }

        ///////////////////////////////////////////////////////////////////////
        bool expand_counter_info(counter_info const& info,
            counter_path_elements& p, discover_counter_func const& f,
            error_code& ec)
        {
            // A '*' wild-card as the instance name is equivalent to no instance
            // name at all.
            if (p.parentinstancename_ == "*")
            {
                HPX_ASSERT(p.parentinstanceindex_ == -1);
                p.parentinstancename_.clear();
            }

            // first expand "locality*"
            if (p.parentinstancename_ == "locality#*")
            {
                counter_info i = info;
                p.parentinstancename_ = "locality";
                return detail::expand_counter_info_localities(i, p, f, ec);
            }

            // now expand "pool#*"
            if (detail::is_pool_kind(p.instancename_))
            {
                bool expand_threads =
                    detail::is_thread_kind(p.subinstancename_);

                counter_info i = info;
                p.instancename_ = detail::get_pool_kind(p.instancename_);
                if (expand_threads)
                {
                    p.subinstancename_ =
                        detail::get_thread_kind(p.subinstancename_) + "-thread";
                }
                return detail::expand_counter_info_pools(
                    expand_threads, i, p, f, ec);
            }
            else if (p.instancename_ == "pool" && p.subinstancename_ == "*")
            {
                p.subinstancename_ = "worker-thread";
                p.subinstanceindex_ = -1;
                counter_info i = info;
                return detail::expand_counter_info_pool_threads(i, p, f, ec);
            }

            if (detail::is_thread_kind(p.instancename_))
            {
                // now expand "<...>-thread#*"
                counter_info i = info;
                p.instancename_ =
                    detail::get_thread_kind(p.instancename_) + "-thread";
                return detail::expand_counter_info_threads(i, p, f, ec);
            }

            // now expand "<...>-node#*"
            if (detail::is_node_kind(p.instancename_))
            {
                counter_info i = info;
                p.instancename_ =
                    detail::get_node_kind(p.instancename_) + "-node";
                return detail::expand_counter_info_nodes(i, p, f, ec);
            }

            // handle wild-cards in aggregate counters
            if (p.parentinstance_is_basename_)
            {
                return detail::expand_basecounter(info, p, f, ec);
            }

            // everything else is handled directly
            return f(info, ec);
        }
    }    // namespace detail

    ///////////////////////////////////////////////////////////////////////////
    /// \brief call the supplied function will all expanded versions of the
    /// supplied counter info.
    bool expand_counter_info(counter_info const& info,
        discover_counter_func const& f, error_code& ec)
    {
        counter_path_elements p;
        counter_status status =
            get_counter_path_elements(info.fullname_, p, ec);
        if (!status_is_valid(status))
            return false;

        return detail::expand_counter_info(info, p, f, ec);
    }

    ///////////////////////////////////////////////////////////////////////////
    static naming::id_type register_with_agas(
        std::string const& fullname, hpx::future<naming::id_type> f)
    {
        // register the canonical name with AGAS
        naming::id_type id = f.get();
        agas::register_name(launch::sync, fullname, id);
        return id;
    }

    ///////////////////////////////////////////////////////////////////////////
    hpx::future<naming::id_type> get_counter_async(
        counter_info const& info, error_code& ec)
    {
        typedef hpx::future<naming::id_type> result_type;

        // complement counter info data
        counter_info complemented_info = info;
        complement_counter_info(complemented_info, ec);
        if (ec)
            result_type();

        ensure_counter_prefix(complemented_info.fullname_);
        // pre-pend prefix, if necessary

        // ask AGAS for the id of the given counter
        naming::id_type id =
            agas::resolve_name(launch::sync, complemented_info.fullname_, ec);
        if (id == naming::invalid_id)
        {
            try
            {
                // figure out target locality
                counter_path_elements p;
                get_counter_path_elements(complemented_info.fullname_, p, ec);
                if (ec)
                    return result_type();

                // Take target locality from base counter if if this is an
                // aggregating counter (the instance name is a base counter).
                if (p.parentinstance_is_basename_)
                {
                    get_counter_path_elements(p.parentinstancename_, p, ec);
                    if (ec)
                        return result_type();
                }

                if (p.parentinstancename_ == "locality" &&
                    (p.parentinstanceindex_ < 0 ||
                        p.parentinstanceindex_ >=
                            static_cast<std::int32_t>(
                                get_num_localities(hpx::launch::sync))))
                {
                    HPX_THROWS_IF(ec, bad_parameter, "get_counter",
                        "attempt to create counter on non-existing locality");
                    return result_type();
                }

                // use the runtime_support component of the target locality to
                // create the new performance counter
                hpx::future<naming::id_type> f;
                if (p.parentinstanceindex_ >= 0)
                {
                    f = create_performance_counter_async(
                        naming::get_id_from_locality_id(
                            static_cast<std::uint32_t>(p.parentinstanceindex_)),
                        complemented_info);
                }
                else
                {
                    f = create_performance_counter_async(
                        naming::get_id_from_locality_id(
                            agas::get_locality_id()),
                        complemented_info);
                }

                // attach the function which registers the id_type with AGAS
                return f.then(hpx::launch::sync,
                    hpx::util::bind_front(
                        &register_with_agas, complemented_info.fullname_));
            }
            catch (hpx::exception const& e)
            {
                if (&ec == &throws)
                    throw;
                ec = make_error_code(e.get_error(), e.what());
                LPCS_(warning).format("failed to create counter {} ({})",
                    remove_counter_prefix(complemented_info.fullname_),
                    e.what());
                return hpx::future<naming::id_type>();
            }
        }
        if (ec)
            return result_type();

        return hpx::make_ready_future(id);
    }

    hpx::future<naming::id_type> get_counter_async(
        std::string name, error_code& ec)
    {
        ensure_counter_prefix(name);    // prepend prefix, if necessary

        counter_info info(name);    // set full counter name
        return get_counter_async(info, ec);
    }

    ///////////////////////////////////////////////////////////////////////////
    void counter_value::serialize(
        serialization::output_archive& ar, const unsigned int)
    {
        // clang-format off
        ar & status_ & time_ & count_ & value_ & scaling_ & scale_inverse_;
        // clang-format on
    }

    void counter_value::serialize(
        serialization::input_archive& ar, const unsigned int)
    {
        // clang-format off
        ar & status_ & time_ & count_ & value_ & scaling_ & scale_inverse_;
        // clang-format on
    }

    void counter_values_array::serialize(
        serialization::output_archive& ar, const unsigned int)
    {
        // clang-format off
        ar & status_ & time_ & count_ & values_ & scaling_ & scale_inverse_;
        // clang-format on
    }

    void counter_values_array::serialize(
        serialization::input_archive& ar, const unsigned int)
    {
        // clang-format off
        ar & status_ & time_ & count_ & values_ & scaling_ & scale_inverse_;
        // clang-format on
    }

    ///////////////////////////////////////////////////////////////////////////
    void counter_type_path_elements::serialize(
        serialization::output_archive& ar, const unsigned int)
    {
        // clang-format off
        ar & objectname_ & countername_ & parameters_;
        // clang-format on
    }

    void counter_type_path_elements::serialize(
        serialization::input_archive& ar, const unsigned int)
    {
        // clang-format off
        ar & objectname_ & countername_ & parameters_;
        // clang-format on
    }

    ///////////////////////////////////////////////////////////////////////////
    void counter_path_elements::serialize(
        serialization::output_archive& ar, const unsigned int)
    {
        typedef counter_type_path_elements base_type;
        hpx::serialization::base_object_type<counter_path_elements, base_type>
            base = hpx::serialization::base_object<base_type>(*this);

        // clang-format off
        ar & base & parentinstancename_ & instancename_ & subinstancename_ &
            parentinstanceindex_ & instanceindex_ & subinstanceindex_ &
            parentinstance_is_basename_;
        // clang-format on
    }

    void counter_path_elements::serialize(
        serialization::input_archive& ar, const unsigned int)
    {
        typedef counter_type_path_elements base_type;
        hpx::serialization::base_object_type<counter_path_elements, base_type>
            base = hpx::serialization::base_object<base_type>(*this);

        // clang-format off
        ar & base & parentinstancename_ & instancename_ & subinstancename_ &
            parentinstanceindex_ & instanceindex_ & subinstanceindex_ &
            parentinstance_is_basename_;
        // clang-format on
    }

    ///////////////////////////////////////////////////////////////////////////
    void counter_info::serialize(
        serialization::output_archive& ar, const unsigned int)
    {
        // clang-format off
        ar & type_ & version_ & status_ & fullname_ & helptext_ &
            unit_of_measure_;
        // clang-format on
    }

    void counter_info::serialize(
        serialization::input_archive& ar, const unsigned int)
    {
        // clang-format off
        ar & type_ & version_ & status_ & fullname_ & helptext_ &
            unit_of_measure_;
        // clang-format on
    }
}}    // namespace hpx::performance_counters
