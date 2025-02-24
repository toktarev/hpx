//  Copyright (c) 2020 Thomas Heller
//  Copyright (c) 2022 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config/constexpr.hpp>
#include <hpx/execution_base/get_env.hpp>
#include <hpx/execution_base/operation_state.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/functional/invoke_result.hpp>
#include <hpx/functional/tag_invoke.hpp>
#include <hpx/functional/traits/is_invocable.hpp>
#include <hpx/type_support/equality.hpp>

#include <cstddef>
#include <exception>
#include <memory>
#include <type_traits>
#include <utility>

namespace hpx { namespace execution { namespace experimental {
#if defined(DOXYGEN)
    /// connect is a customization point object.
    /// For some subexpression `s` and `r`, let `S` be the type such that `decltype((s))`
    /// is `S` and let `R` be the type such that `decltype((r))` is `R`. The result of
    /// the expression `hpx::execution::experimental::connect(s, r)` is then equivalent to:
    ///     * `s.connect(r)`, if that expression is valid and returns a type
    ///       satisfying the `operation_state`
    ///       (\see hpx::execution::experimental::traits::is_operation_state)
    ///       and if `S` satisfies the `sender` concept.
    ///     * `s.connect(r)`, if that expression is valid and returns a type
    ///       satisfying the `operation_state`
    ///       (\see hpx::execution::experimental::traits::is_operation_state)
    ///       and if `S` satisfies the `sender` concept.
    ///       Overload resolution is performed in a context that include the declaration
    ///       `void connect();`
    ///     * Otherwise, the expression is ill-formed.
    ///
    /// The customization is implemented in terms of
    /// `hpx::functional::tag_invoke`.
    template <typename S, typename R>
    void connect(S&& s, R&& r);

    /// The name schedule denotes a customization point object. For some
    /// subexpression s, let S be decltype((s)). The expression schedule(s) is
    /// expression-equivalent to:
    ///
    ///     * s.schedule(), if that expression is valid and its type models
    ///       sender.
    ///     * Otherwise, schedule(s), if that expression is valid and its type
    ///       models sender with overload resolution performed in a context that
    ///       includes the declaration
    ///
    ///           void schedule();
    ///
    ///       and that does not include a declaration of schedule.
    ///
    ///      * Otherwise, schedule(s) is ill-formed.
    ///
    /// The customization is implemented in terms of
    /// `hpx::functional::tag_invoke`.

#endif

    /// A sender is a type that is describing an asynchronous operation. The
    /// operation itself might not have started yet. In order to get the result
    /// of this asynchronous operation, a sender needs to be connected to a
    /// receiver with the corresponding value, error and done channels:
    ///     * `hpx::execution::experimental::connect`
    ///
    /// In addition, `hpx::execution::experimental::sender_traits ` needs to
    /// be specialized in some form.
    ///
    /// A sender's destructor shall not block pending completion of submitted
    /// operations.
    template <typename Sender, typename Env = no_env>
    struct is_sender;

    /// \see is_sender
    template <typename Sender, typename Receiver>
    struct is_sender_to;

    /// `sender_traits` expose the different value and error types exposed
    /// by a sender. This can be either specialized directly for user defined
    /// sender types or embedded value_types, error_types and sends_done
    /// inside the sender type can be provided.
    template <typename Sender, typename Env = empty_env>
    struct sender_traits;

    template <typename Sender, typename Env>
    struct sender_traits<Sender volatile, Env> : sender_traits<Sender, Env>
    {
    };
    template <typename Sender, typename Env>
    struct sender_traits<Sender const, Env> : sender_traits<Sender, Env>
    {
    };
    template <typename Sender, typename Env>
    struct sender_traits<Sender&, Env> : sender_traits<Sender, Env>
    {
    };
    template <typename Sender, typename Env>
    struct sender_traits<Sender&&, Env> : sender_traits<Sender, Env>
    {
    };

    namespace detail {
        template <typename Sender>
        constexpr bool specialized(...)
        {
            return true;
        }

        template <typename Sender>
        constexpr bool specialized(
            typename sender_traits<Sender>::__unspecialized*)
        {
            return false;
        }
    }    // namespace detail

    template <typename Sender, typename Env>
    struct is_sender
      : std::integral_constant<bool,
            std::is_move_constructible_v<std::decay_t<Sender>> &&
                detail::specialized<std::decay_t<Sender>>(nullptr)>
    {
    };

    template <typename Sender>
    inline constexpr bool is_sender_v = is_sender<Sender>::value;

    struct invocable_archetype
    {
        void operator()() {}
    };

    namespace detail {
        template <typename S, typename R, typename Enable = void>
        struct has_member_connect : std::false_type
        {
        };

        // different versions of clang-format disagree
        // clang-format off
        template <typename S, typename R>
        struct has_member_connect<S, R,
            hpx::util::always_void_t<decltype(
                std::declval<S>().connect(std::declval<R>()))>> : std::true_type
        {
        };
        // clang-format on
    }    // namespace detail

    HPX_HOST_DEVICE_INLINE_CONSTEXPR_VARIABLE
    struct connect_t : hpx::functional::tag<connect_t>
    {
    } connect{};

    namespace detail {
        template <typename S, typename R, typename Enable = void>
        struct connect_result_helper
        {
            struct dummy_operation_state
            {
            };
            using type = dummy_operation_state;
        };

        template <typename S, typename R>
        struct connect_result_helper<S, R,
            std::enable_if_t<hpx::is_invocable<connect_t, S, R>::value>>
          : hpx::util::invoke_result<connect_t, S, R>
        {
        };
    }    // namespace detail

    namespace detail {
        template <typename F, typename E>
        struct as_receiver
        {
            F f;

            void set_value() noexcept(noexcept(HPX_INVOKE(f, )))
            {
                HPX_INVOKE(f, );
            }

            template <typename E_>
            HPX_NORETURN void set_error(E_&&) noexcept
            {
                std::terminate();
            }

            void set_stopped() noexcept {}
        };
    }    // namespace detail

    namespace detail {
        template <typename S, typename Enable = void>
        struct has_member_schedule : std::false_type
        {
        };

        template <typename S>
        struct has_member_schedule<S,
            hpx::util::always_void_t<decltype(std::declval<S>().schedule())>>
          : std::true_type
        {
        };
    }    // namespace detail

    HPX_HOST_DEVICE_INLINE_CONSTEXPR_VARIABLE
    struct schedule_t : hpx::functional::tag<schedule_t>
    {
    } schedule{};

    namespace detail {
        template <bool IsSenderReceiver, typename Sender, typename Receiver>
        struct is_sender_to_impl;

        template <typename Sender, typename Receiver>
        struct is_sender_to_impl<false, Sender, Receiver> : std::false_type
        {
        };

        template <typename Sender, typename Receiver>
        struct is_sender_to_impl<true, Sender, Receiver>
          : std::integral_constant<bool,
                hpx::is_invocable_v<connect_t, Sender&&, Receiver&&> ||
                    hpx::is_invocable_v<connect_t, Sender&&, Receiver&> ||
                    hpx::is_invocable_v<connect_t, Sender&&, Receiver const&> ||
                    hpx::is_invocable_v<connect_t, Sender&, Receiver&&> ||
                    hpx::is_invocable_v<connect_t, Sender&, Receiver&> ||
                    hpx::is_invocable_v<connect_t, Sender&, Receiver const&> ||
                    hpx::is_invocable_v<connect_t, Sender const&, Receiver&&> ||
                    hpx::is_invocable_v<connect_t, Sender const&, Receiver&> ||
                    hpx::is_invocable_v<connect_t, Sender const&,
                        Receiver const&>>
        {
        };
    }    // namespace detail

    template <typename Sender, typename Receiver>
    struct is_sender_to
      : detail::is_sender_to_impl<
            is_sender_v<Sender> && is_receiver_v<Receiver>, Sender, Receiver>
    {
    };

    template <typename Sender, typename Receiver>
    inline constexpr bool is_sender_to_v =
        is_sender_to<Sender, Receiver>::value;

    namespace detail {
        template <typename... As>
        struct tuple_mock;
        template <typename... As>
        struct variant_mock;

        template <typename Sender>
        constexpr bool has_value_types(
            typename Sender::template value_types<tuple_mock, variant_mock>*)
        {
            return true;
        }

        template <typename Sender>
        constexpr bool has_value_types(...)
        {
            return false;
        }

        template <typename Sender>
        constexpr bool has_error_types(
            typename Sender::template error_types<variant_mock>*)
        {
            return true;
        }

        template <typename Sender>
        constexpr bool has_error_types(...)
        {
            return false;
        }

        template <typename Sender>
        constexpr bool has_sends_done(decltype(Sender::sends_done)*)
        {
            return true;
        }

        template <typename Sender>
        constexpr bool has_sends_done(...)
        {
            return false;
        }

        template <typename Sender>
        struct has_sender_types
          : std::integral_constant<bool,
                has_value_types<Sender>(nullptr) &&
                    has_error_types<Sender>(nullptr) &&
                    has_sends_done<Sender>(nullptr)>
        {
        };

        template <bool HasSenderTraits, typename Sender, typename Env>
        struct sender_traits_base;

        template <typename Sender, typename Env>
        struct sender_traits_base<true /* HasSenderTraits */, Sender, Env>
        {
            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types =
                typename Sender::template value_types<Tuple, Variant>;

            template <template <typename...> class Variant>
            using error_types = typename Sender::template error_types<Variant>;

            static constexpr bool sends_done = Sender::sends_done;
        };

        template <typename Sender, typename Env>
        struct sender_traits_base<false /* HasSenderTraits */, Sender, Env>
        {
            using __unspecialized = void;
        };

        template <typename Sender>
        struct is_typed_sender
          : std::integral_constant<bool,
                is_sender<Sender>::value &&
                    detail::has_sender_types<Sender>::value>
        {
        };
    }    // namespace detail

    template <typename Sender, typename Env>
    struct sender_traits
      : detail::sender_traits_base<detail::has_sender_types<Sender>::value,
            Sender, Env>
    {
    };

    // Explicitly specialize for void to avoid forming references to void
    // (is_invocable is in the base implementation, which forms a reference to
    // the Sender type).
    template <>
    struct sender_traits<void>
    {
        using __unspecialized = void;
    };

    namespace detail {
        template <template <typename...> class Tuple,
            template <typename...> class Variant>
        struct value_types
        {
            template <typename Sender>
            struct apply
            {
                using type =
                    typename hpx::execution::experimental::sender_traits<
                        Sender>::template value_types<Tuple, Variant>;
            };
        };

        template <template <typename...> class Variant>
        struct error_types
        {
            template <typename Sender>
            struct apply
            {
                using type =
                    typename hpx::execution::experimental::sender_traits<
                        Sender>::template error_types<Variant>;
            };
        };
    }    // namespace detail

    template <typename Scheduler, typename Enable = void>
    struct is_scheduler : std::false_type
    {
    };

    template <typename Scheduler>
    struct is_scheduler<Scheduler,
        std::enable_if_t<hpx::is_invocable_v<schedule_t, Scheduler> &&
            std::is_copy_constructible_v<Scheduler> &&
            hpx::traits::is_equality_comparable_v<Scheduler>>> : std::true_type
    {
    };

    template <typename Scheduler>
    inline constexpr bool is_scheduler_v = is_scheduler<Scheduler>::value;

    template <typename S, typename R>
    using connect_result_t = hpx::util::invoke_result_t<connect_t, S, R>;
}}}    // namespace hpx::execution::experimental
