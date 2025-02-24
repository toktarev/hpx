//  Copyright (c) 2011-2012 Thomas Heller
//  Copyright (c) 2013-2019 Agustin Berge
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/datastructures/member_pack.hpp>
#include <hpx/functional/invoke.hpp>
#include <hpx/functional/invoke_result.hpp>
#include <hpx/functional/one_shot.hpp>
#include <hpx/functional/traits/get_function_address.hpp>
#include <hpx/functional/traits/get_function_annotation.hpp>
#include <hpx/type_support/decay.hpp>
#include <hpx/type_support/pack.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace hpx { namespace util {
    namespace detail {
        template <typename F, typename Ts, typename... Us>
        struct invoke_bound_front_result;

        template <typename F, typename... Ts, typename... Us>
        struct invoke_bound_front_result<F, util::pack<Ts...>, Us...>
          : util::invoke_result<F, Ts..., Us...>
        {
        };

        ///////////////////////////////////////////////////////////////////////
        template <typename F, typename Is, typename... Ts>
        class bound_front;

        template <typename F, std::size_t... Is, typename... Ts>
        class bound_front<F, index_pack<Is...>, Ts...>
        {
        public:
            bound_front() = default;    // needed for serialization

            template <typename F_, typename... Ts_,
                typename = typename std::enable_if<
                    std::is_constructible<F, F_>::value>::type>
            constexpr explicit bound_front(F_&& f, Ts_&&... vs)
              : _f(HPX_FORWARD(F_, f))
              , _args(std::piecewise_construct, HPX_FORWARD(Ts_, vs)...)
            {
            }

#if !defined(__NVCC__) && !defined(__CUDACC__)
            bound_front(bound_front const&) = default;
            bound_front(bound_front&&) = default;
#else
            constexpr HPX_HOST_DEVICE bound_front(bound_front const& other)
              : _f(other._f)
              , _args(other._args)
            {
            }

            constexpr HPX_HOST_DEVICE bound_front(bound_front&& other)
              : _f(HPX_MOVE(other._f))
              , _args(HPX_MOVE(other._args))
            {
            }
#endif

            bound_front& operator=(bound_front const&) = delete;

            template <typename... Us>
            constexpr HPX_HOST_DEVICE typename invoke_bound_front_result<F&,
                util::pack<Ts&...>, Us&&...>::type
            operator()(Us&&... vs) &
            {
                return HPX_INVOKE(
                    _f, _args.template get<Is>()..., HPX_FORWARD(Us, vs)...);
            }

            template <typename... Us>
            constexpr HPX_HOST_DEVICE
                typename invoke_bound_front_result<F const&,
                    util::pack<Ts const&...>, Us&&...>::type
                operator()(Us&&... vs) const&
            {
                return HPX_INVOKE(
                    _f, _args.template get<Is>()..., HPX_FORWARD(Us, vs)...);
            }

            template <typename... Us>
            constexpr HPX_HOST_DEVICE typename invoke_bound_front_result<F&&,
                util::pack<Ts&&...>, Us&&...>::type
            operator()(Us&&... vs) &&
            {
                return HPX_INVOKE(HPX_MOVE(_f),
                    HPX_MOVE(_args).template get<Is>()...,
                    HPX_FORWARD(Us, vs)...);
            }

            template <typename... Us>
            constexpr HPX_HOST_DEVICE
                typename invoke_bound_front_result<F const&&,
                    util::pack<Ts const&&...>, Us&&...>::type
                operator()(Us&&... vs) const&&
            {
                return HPX_INVOKE(HPX_MOVE(_f),
                    HPX_MOVE(_args).template get<Is>()...,
                    HPX_FORWARD(Us, vs)...);
            }

            template <typename Archive>
            void serialize(Archive& ar, unsigned int const /*version*/)
            {
                ar& _f;
                ar& _args;
            }

            constexpr std::size_t get_function_address() const
            {
                return traits::get_function_address<F>::call(_f);
            }

            constexpr char const* get_function_annotation() const
            {
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
                return traits::get_function_annotation<F>::call(_f);
#else
                return nullptr;
#endif
            }

#if HPX_HAVE_ITTNOTIFY != 0 && !defined(HPX_HAVE_APEX)
            util::itt::string_handle get_function_annotation_itt() const
            {
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
                return traits::get_function_annotation_itt<F>::call(_f);
#else
                static util::itt::string_handle sh("bound_front");
                return sh;
#endif
            }
#endif

        private:
            F _f;
            util::member_pack_for<Ts...> _args;
        };
    }    // namespace detail

    template <typename F, typename... Ts>
    constexpr detail::bound_front<typename std::decay<F>::type,
        typename util::make_index_pack<sizeof...(Ts)>::type,
        typename util::decay_unwrap<Ts>::type...>
    bind_front(F&& f, Ts&&... vs)
    {
        typedef detail::bound_front<typename std::decay<F>::type,
            typename util::make_index_pack<sizeof...(Ts)>::type,
            typename util::decay_unwrap<Ts>::type...>
            result_type;

        return result_type(HPX_FORWARD(F, f), HPX_FORWARD(Ts, vs)...);
    }

    // nullary functions do not need to be bound again
    template <typename F>
    constexpr typename std::decay<F>::type bind_front(F&& f)
    {
        return HPX_FORWARD(F, f);
    }
}}    // namespace hpx::util

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace traits {
    ///////////////////////////////////////////////////////////////////////////
#if defined(HPX_HAVE_THREAD_DESCRIPTION)
    template <typename F, typename... Ts>
    struct get_function_address<util::detail::bound_front<F, Ts...>>
    {
        static constexpr std::size_t call(
            util::detail::bound_front<F, Ts...> const& f) noexcept
        {
            return f.get_function_address();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename F, typename... Ts>
    struct get_function_annotation<util::detail::bound_front<F, Ts...>>
    {
        static constexpr char const* call(
            util::detail::bound_front<F, Ts...> const& f) noexcept
        {
            return f.get_function_annotation();
        }
    };

#if HPX_HAVE_ITTNOTIFY != 0 && !defined(HPX_HAVE_APEX)
    template <typename F, typename... Ts>
    struct get_function_annotation_itt<util::detail::bound_front<F, Ts...>>
    {
        static util::itt::string_handle call(
            util::detail::bound_front<F, Ts...> const& f) noexcept
        {
            return f.get_function_annotation_itt();
        }
    };
#endif
#endif
}}    // namespace hpx::traits

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace serialization {
    // serialization of the bound_front object
    template <typename Archive, typename F, typename... Ts>
    void serialize(Archive& ar,
        ::hpx::util::detail::bound_front<F, Ts...>& bound,
        unsigned int const version = 0)
    {
        bound.serialize(ar, version);
    }
}}    // namespace hpx::serialization
