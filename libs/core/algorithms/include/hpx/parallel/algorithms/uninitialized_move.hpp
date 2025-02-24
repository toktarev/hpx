//  Copyright (c) 2014-2017 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/algorithms/uninitialized_move.hpp

#pragma once

#if defined(DOXYGEN)
namespace hpx {
    /// Moves the elements in the range, defined by [first, last), to an
    /// uninitialized memory area beginning at \a dest. If an exception is
    /// thrown during the initialization, some objects in [first, last) are
    /// left in a valid but unspecified state.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first assignments.
    ///
    /// \tparam InIter      The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     input iterator.
    /// \tparam FwdIter     The type of the iterator representing the
    ///                     destination range (deduced).
    ///                     This iterator type must meet the requirements of a
    ///                     forward iterator.
    ///
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    /// \param dest         Refers to the beginning of the destination range.
    ///
    /// The assignments in the parallel \a uninitialized_move algorithm invoked
    /// without an execution policy object will execute in sequential order in
    /// the calling thread.
    ///
    /// \returns  The \a uninitialized_move algorithm returns \a FwdIter.
    ///           The \a uninitialized_move algorithm returns the output
    ///           iterator to the element in the destination range, one past
    ///           the last element moved.
    ///
    template <typename InIter, typename FwdIter>
    FwdIter uninitialized_move(InIter first, InIter last, FwdIter dest);

    /// Moves the elements in the range, defined by [first, last), to an
    /// uninitialized memory area beginning at \a dest. If an exception is
    /// thrown during the initialization, some objects in [first, last) are
    /// left in a valid but unspecified state.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first assignments.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam FwdIter1    The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     forward iterator.
    /// \tparam FwdIter2    The type of the iterator representing the
    ///                     destination range (deduced).
    ///                     This iterator type must meet the requirements of a
    ///                     forward iterator.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    /// \param dest         Refers to the beginning of the destination range.
    ///
    /// The assignments in the parallel \a uninitialized_move algorithm invoked
    /// with an execution policy object of type \a sequenced_policy
    /// execute in sequential order in the calling thread.
    ///
    /// The assignments in the parallel \a uninitialized_move algorithm invoked
    /// with an execution policy object of type \a parallel_policy or
    /// \a parallel_task_policy are permitted to execute in an
    /// unordered fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a uninitialized_move algorithm returns a
    ///           \a hpx::future<FwdIter2>, if the execution policy is of type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy and
    ///           returns \a FwdIter2 otherwise.
    ///           The \a uninitialized_move algorithm returns the output
    ///           iterator to the element in the destination range, one past
    ///           the last element moved.
    ///
    template <typename ExPolicy, typename FwdIter1, typename FwdIter2>
    typename parallel::util::detail::algorithm_result<ExPolicy, FwdIter2>::type
    uninitialized_move(
        ExPolicy&& policy, FwdIter1 first, FwdIter1 last, FwdIter2 dest);

    /// Moves the elements in the range [first, first + count), starting from
    /// first and proceeding to first + count - 1., to another range beginning
    /// at dest. If an exception is
    /// thrown during the initialization, some objects in [first, first + count)
    /// are left in a valid but unspecified state.
    ///
    /// \note   Complexity: Performs exactly \a count movements, if
    ///         count > 0, no move operations otherwise.
    ///
    /// \tparam FwdIter1      The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     input iterator.
    /// \tparam Size        The type of the argument specifying the number of
    ///                     elements to apply \a f to.
    /// \tparam FwdIter2     The type of the iterator representing the
    ///                     destination range (deduced).
    ///                     This iterator type must meet the requirements of a
    ///                     forward iterator.
    ///
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param count        Refers to the number of elements starting at
    ///                     \a first the algorithm will be applied to.
    /// \param dest         Refers to the beginning of the destination range.
    ///
    /// The assignments in the parallel \a uninitialized_move_n algorithm
    /// invoked with an execution policy object of type
    /// \a sequenced_policy execute in sequential order in the
    /// calling thread.
    ///
    /// \returns  The \a uninitialized_move_n algorithm returns a
    ///           returns \a FwdIter2.
    ///           The \a uninitialized_move_n algorithm returns the output
    ///           iterator to the element in the destination range, one past
    ///           the last element moved.
    ///
    template <typename InIter, typename Size, typename FwdIter>
    FwdIter uninitialized_move_n(InIter first, Size count, FwdIter dest);

    /// Moves the elements in the range [first, first + count), starting from
    /// first and proceeding to first + count - 1., to another range beginning
    /// at dest. If an exception is
    /// thrown during the initialization, some objects in [first, first + count)
    /// are left in a valid but unspecified state.
    ///
    /// \note   Complexity: Performs exactly \a count movements, if
    ///         count > 0, no move operations otherwise.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam FwdIter1      The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     input iterator.
    /// \tparam Size        The type of the argument specifying the number of
    ///                     elements to apply \a f to.
    /// \tparam FwdIter2     The type of the iterator representing the
    ///                     destination range (deduced).
    ///                     This iterator type must meet the requirements of a
    ///                     forward iterator.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param count        Refers to the number of elements starting at
    ///                     \a first the algorithm will be applied to.
    /// \param dest         Refers to the beginning of the destination range.
    ///
    /// The assignments in the parallel \a uninitialized_move_n algorithm
    /// invoked with an execution policy object of type
    /// \a sequenced_policy execute in sequential order in the
    /// calling thread.
    ///
    /// The assignments in the parallel \a uninitialized_move_n algorithm
    /// invoked with an execution policy object of type
    /// \a parallel_policy or
    /// \a parallel_task_policy are permitted to execute in an
    /// unordered fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a uninitialized_move_n algorithm returns a
    ///           \a hpx::future<FwdIter2> if the execution policy is of type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy and
    ///           returns \a FwdIter2 otherwise.
    ///           The \a uninitialized_move_n algorithm returns the output
    ///           iterator to the element in the destination range, one past
    ///           the last element moved.
    ///
    template <typename ExPolicy, typename FwdIter1, typename Size,
        typename FwdIter2>
    typename parallel::util::detail::algorithm_result<ExPolicy, FwdIter2>::type
    uninitialized_move_n(
        ExPolicy&& policy, FwdIter1 first, Size count, FwdIter2 dest);
}    // namespace hpx

#else    // DOXYGEN

#include <hpx/config.hpp>
#include <hpx/concepts/concepts.hpp>
#include <hpx/functional/detail/tag_fallback_invoke.hpp>
#include <hpx/iterator_support/traits/is_iterator.hpp>

#include <hpx/execution/algorithms/detail/is_negative.hpp>
#include <hpx/executors/execution_policy.hpp>
#include <hpx/parallel/algorithms/detail/dispatch.hpp>
#include <hpx/parallel/algorithms/detail/distance.hpp>
#include <hpx/parallel/util/detail/algorithm_result.hpp>
#include <hpx/parallel/util/detail/sender_util.hpp>
#include <hpx/parallel/util/loop.hpp>
#include <hpx/parallel/util/partitioner_with_cleanup.hpp>
#include <hpx/parallel/util/result_types.hpp>
#include <hpx/parallel/util/zip_iterator.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace hpx { namespace parallel { inline namespace v1 {
    ///////////////////////////////////////////////////////////////////////////
    // uninitialized_move
    namespace detail {
        /// \cond NOINTERNAL

        ///////////////////////////////////////////////////////////////////////
        template <typename InIter1, typename FwdIter2, typename Cond>
        util::in_out_result<InIter1, FwdIter2> sequential_uninitialized_move(
            InIter1 first, FwdIter2 dest, Cond cond)
        {
            using value_type =
                typename std::iterator_traits<FwdIter2>::value_type;

            FwdIter2 current = dest;
            try
            {
                for (/* */; HPX_INVOKE(cond, first, current);
                     (void) ++first, ++current)
                {
                    ::new (std::addressof(*current))
                        value_type(HPX_MOVE(*first));
                }
                return util::in_out_result<InIter1, FwdIter2>{first, current};
            }
            catch (...)
            {
                for (/* */; dest != current; ++dest)
                {
                    (*dest).~value_type();
                }
                throw;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename InIter1, typename InIter2>
        util::in_out_result<InIter1, InIter2> sequential_uninitialized_move_n(
            InIter1 first, std::size_t count, InIter2 dest,
            util::cancellation_token<util::detail::no_data>& tok)
        {
            using value_type =
                typename std::iterator_traits<InIter2>::value_type;

            return util::in_out_result<InIter1, InIter2>{
                std::next(first, count),
                util::loop_with_cleanup_n_with_token(
                    first, count, dest, tok,
                    [](InIter1 it, InIter2 dest) -> void {
                        ::new (std::addressof(*dest)) value_type(HPX_MOVE(*it));
                    },
                    [](InIter2 dest) -> void { (*dest).~value_type(); })};
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename ExPolicy, typename Iter, typename FwdIter2>
        typename util::detail::algorithm_result<ExPolicy,
            util::in_out_result<Iter, FwdIter2>>::type
        parallel_sequential_uninitialized_move_n(
            ExPolicy&& policy, Iter first, std::size_t count, FwdIter2 dest)
        {
            if (count == 0)
            {
                return util::detail::algorithm_result<ExPolicy,
                    util::in_out_result<Iter, FwdIter2>>::
                    get(util::in_out_result<Iter, FwdIter2>{first, dest});
            }

            typedef hpx::util::zip_iterator<Iter, FwdIter2> zip_iterator;
            typedef std::pair<FwdIter2, FwdIter2> partition_result_type;
            typedef
                typename std::iterator_traits<FwdIter2>::value_type value_type;

            util::cancellation_token<util::detail::no_data> tok;
            return util::partitioner_with_cleanup<ExPolicy,
                util::in_out_result<Iter, FwdIter2>, partition_result_type>::
                call(
                    HPX_FORWARD(ExPolicy, policy),
                    hpx::util::make_zip_iterator(first, dest), count,
                    [tok](zip_iterator t, std::size_t part_size) mutable
                    -> partition_result_type {
                        using hpx::get;
                        auto iters = t.get_iterator_tuple();
                        FwdIter2 dest = get<1>(iters);
                        return std::make_pair(dest,
                            util::get_second_element(
                                sequential_uninitialized_move_n(
                                    get<0>(iters), part_size, dest, tok)));
                    },
                    // finalize, called once if no error occurred
                    [first, dest, count](
                        std::vector<hpx::future<partition_result_type>>&&
                            data) mutable
                    -> util::in_out_result<Iter, FwdIter2> {
                        // make sure iterators embedded in function object that is
                        // attached to futures are invalidated
                        data.clear();

                        std::advance(first, count);
                        std::advance(dest, count);
                        return util::in_out_result<Iter, FwdIter2>{first, dest};
                    },
                    // cleanup function, called for each partition which
                    // didn't fail, but only if at least one failed
                    [](partition_result_type&& r) -> void {
                        while (r.first != r.second)
                        {
                            (*r.first).~value_type();
                            ++r.first;
                        }
                    });
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename IterPair>
        struct uninitialized_move
          : public detail::algorithm<uninitialized_move<IterPair>, IterPair>
        {
            uninitialized_move()
              : uninitialized_move::algorithm("uninitialized_move")
            {
            }

            template <typename ExPolicy, typename InIter1, typename Sent,
                typename FwdIter2>
            static util::in_out_result<InIter1, FwdIter2> sequential(
                ExPolicy, InIter1 first, Sent last, FwdIter2 dest)
            {
                return sequential_uninitialized_move(
                    first, dest, [last](InIter1 first, FwdIter2) -> bool {
                        return first != last;
                    });
            }

            template <typename ExPolicy, typename Iter, typename Sent,
                typename FwdIter2>
            static typename util::detail::algorithm_result<ExPolicy,
                util::in_out_result<Iter, FwdIter2>>::type
            parallel(ExPolicy&& policy, Iter first, Sent last, FwdIter2 dest)
            {
                return parallel_sequential_uninitialized_move_n(
                    HPX_FORWARD(ExPolicy, policy), first,
                    detail::distance(first, last), dest);
            }
        };
        /// \endcond
    }    // namespace detail

    /// Moves the elements in the range, defined by [first, last), to an
    /// uninitialized memory area beginning at \a dest. If an exception is
    /// thrown during the initialization, some objects in [first, last) are
    /// left in a valid but unspecified state.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first move operations.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam FwdIter1    The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     forward iterator.
    /// \tparam FwdIter2    The type of the iterator representing the
    ///                     destination range (deduced).
    ///                     This iterator type must meet the requirements of a
    ///                     forward iterator.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    /// \param dest         Refers to the beginning of the destination range.
    ///
    /// The assignments in the parallel \a uninitialized_move algorithm invoked
    /// with an execution policy object of type \a sequenced_policy
    /// execute in sequential order in the calling thread.
    ///
    /// The assignments in the parallel \a uninitialized_move algorithm invoked
    /// with an execution policy object of type \a parallel_policy or
    /// \a parallel_task_policy are permitted to execute in an
    /// unordered fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a uninitialized_move algorithm returns a
    ///           \a hpx::future<FwdIter2>, if the execution policy is of type
    ///           \a sequenced_task_policy or
    ///           \a parallel_task_policy and
    ///           returns \a FwdIter2 otherwise.
    ///           The \a uninitialized_move algorithm returns the output
    ///           iterator to the element in the destination range, one past
    ///           the last element moved.
    ///
    template <typename ExPolicy, typename FwdIter1, typename FwdIter2,
        HPX_CONCEPT_REQUIRES_(hpx::is_execution_policy<ExPolicy>::value&&
                hpx::traits::is_iterator<FwdIter1>::value&&
                    hpx::traits::is_iterator<FwdIter2>::value)>
    HPX_DEPRECATED_V(1, 7,
        "hpx::parallel::uninitialized_move is deprecated, use "
        "hpx::uninitialized_move "
        "instead")
    typename util::detail::algorithm_result<ExPolicy, FwdIter2>::type
        uninitialized_move(
            ExPolicy&& policy, FwdIter1 first, FwdIter1 last, FwdIter2 dest)
    {
        static_assert((hpx::traits::is_forward_iterator<FwdIter1>::value),
            "Required at least forward iterator.");
        static_assert((hpx::traits::is_forward_iterator<FwdIter2>::value),
            "Requires at least forward iterator.");

#if defined(HPX_GCC_VERSION) && HPX_GCC_VERSION >= 100000
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
        return parallel::util::get_second_element(
            detail::uninitialized_move<
                parallel::util::in_out_result<FwdIter1, FwdIter2>>()
                .call(HPX_FORWARD(ExPolicy, policy), first, last, dest));
#if defined(HPX_GCC_VERSION) && HPX_GCC_VERSION >= 100000
#pragma GCC diagnostic pop
#endif
    }

    /////////////////////////////////////////////////////////////////////////////
    // uninitialized_move_sent
    namespace detail {
        /// \cond NOINTERNAL
        template <typename IterPair>
        struct uninitialized_move_sent
          : public detail::algorithm<uninitialized_move_sent<IterPair>,
                IterPair>
        {
            uninitialized_move_sent()
              : uninitialized_move_sent::algorithm("uninitialized_move_sent")
            {
            }

            template <typename ExPolicy, typename InIter1, typename Sent1,
                typename FwdIter2, typename Sent2>
            static util::in_out_result<InIter1, FwdIter2> sequential(ExPolicy,
                InIter1 first, Sent1 last, FwdIter2 dest, Sent2 last_d)
            {
                return sequential_uninitialized_move(first, dest,
                    [last, last_d](InIter1 first, FwdIter2 current) -> bool {
                        return !(first == last || current == last_d);
                    });
            }

            template <typename ExPolicy, typename Iter, typename Sent1,
                typename FwdIter2, typename Sent2>
            static typename util::detail::algorithm_result<ExPolicy,
                util::in_out_result<Iter, FwdIter2>>::type
            parallel(ExPolicy&& policy, Iter first, Sent1 last, FwdIter2 dest,
                Sent2 last_d)
            {
                std::size_t dist1 = detail::distance(first, last);
                std::size_t dist2 = detail::distance(dest, last_d);
                std::size_t dist = dist1 <= dist2 ? dist1 : dist2;

                return parallel_sequential_uninitialized_move_n(
                    HPX_FORWARD(ExPolicy, policy), first, dist, dest);
            }
        };
        /// \endcond
    }    // namespace detail

    /////////////////////////////////////////////////////////////////////////////
    // uninitialized_move_n
    namespace detail {
        /// \cond NOINTERNAL

        // provide our own implementation of std::uninitialized_move_n as some
        // versions of MSVC horribly fail at compiling it for some types T
        template <typename InIter1, typename InIter2>
        util::in_out_result<InIter1, InIter2> std_uninitialized_move_n(
            InIter1 first, std::size_t count, InIter2 d_first)
        {
            using value_type =
                typename std::iterator_traits<InIter2>::value_type;

            InIter2 current = d_first;
            try
            {
                for (/* */; count != 0; ++first, (void) ++current, --count)
                {
                    ::new (std::addressof(*current))
                        value_type(HPX_MOVE(*first));
                }
                return util::in_out_result<InIter1, InIter2>{first, current};
            }
            catch (...)
            {
                for (/* */; d_first != current; ++d_first)
                {
                    (*d_first).~value_type();
                }
                throw;
            }
        }

        template <typename IterPair>
        struct uninitialized_move_n
          : public detail::algorithm<uninitialized_move_n<IterPair>, IterPair>
        {
            uninitialized_move_n()
              : uninitialized_move_n::algorithm("uninitialized_move_n")
            {
            }

            template <typename ExPolicy, typename InIter1, typename InIter2>
            static IterPair sequential(
                ExPolicy, InIter1 first, std::size_t count, InIter2 dest)
            {
                return std_uninitialized_move_n(first, count, dest);
            }

            template <typename ExPolicy, typename Iter, typename FwdIter2>
            static typename util::detail::algorithm_result<ExPolicy,
                IterPair>::type
            parallel(
                ExPolicy&& policy, Iter first, std::size_t count, FwdIter2 dest)
            {
                return parallel_sequential_uninitialized_move_n(
                    HPX_FORWARD(ExPolicy, policy), first, count, dest);
            }
        };
        /// \endcond
    }    // namespace detail

    template <typename ExPolicy, typename FwdIter1, typename Size,
        typename FwdIter2,
        HPX_CONCEPT_REQUIRES_(hpx::is_execution_policy<ExPolicy>::value&&
                hpx::traits::is_iterator<FwdIter1>::value&&
                    hpx::traits::is_iterator<FwdIter2>::value)>
    HPX_DEPRECATED_V(1, 7,
        "hpx::parallel::uninitialized_move_n is deprecated, use "
        "hpx::uninitialized_move_n "
        "instead")
    typename util::detail::algorithm_result<ExPolicy,
        std::pair<FwdIter1, FwdIter2>>::type
        uninitialized_move_n(
            ExPolicy&& policy, FwdIter1 first, Size count, FwdIter2 dest)
    {
        static_assert(hpx::traits::is_forward_iterator<FwdIter1>::value,
            "Required at least forward iterator.");
        static_assert(hpx::traits::is_forward_iterator<FwdIter2>::value,
            "Requires at least forward iterator.");

        // if count is representing a negative value, we do nothing
        if (detail::is_negative(count))
        {
            return parallel::util::detail::algorithm_result<ExPolicy,
                std::pair<FwdIter1, FwdIter2>>::get(std::make_pair(first,
                dest));
        }

#if defined(HPX_GCC_VERSION) && HPX_GCC_VERSION >= 100000
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
        return parallel::util::get_pair(
            parallel::v1::detail::uninitialized_move_n<
                parallel::util::in_out_result<FwdIter1, FwdIter2>>()
                .call(HPX_FORWARD(ExPolicy, policy), first, std::size_t(count),
                    dest));
#if defined(HPX_GCC_VERSION) && HPX_GCC_VERSION >= 100000
#pragma GCC diagnostic pop
#endif
    }
}}}    // namespace hpx::parallel::v1

namespace hpx {
    ///////////////////////////////////////////////////////////////////////////
    // DPO for hpx::uninitialized_move
    inline constexpr struct uninitialized_move_t final
      : hpx::detail::tag_parallel_algorithm<uninitialized_move_t>
    {
        // clang-format off
        template <typename InIter, typename FwdIter,
            HPX_CONCEPT_REQUIRES_(
                hpx::traits::is_iterator<InIter>::value &&
                hpx::traits::is_forward_iterator<FwdIter>::value
            )>
        // clang-format on
        friend FwdIter tag_fallback_invoke(
            hpx::uninitialized_move_t, InIter first, InIter last, FwdIter dest)
        {
            static_assert(hpx::traits::is_input_iterator<InIter>::value,
                "Required at least input iterator.");
            static_assert(hpx::traits::is_forward_iterator<FwdIter>::value,
                "Requires at least forward iterator.");

            return parallel::util::get_second_element(
                hpx::parallel::v1::detail::uninitialized_move<
                    parallel::util::in_out_result<InIter, FwdIter>>()
                    .call(hpx::execution::seq, first, last, dest));
        }

        // clang-format off
        template <typename ExPolicy, typename FwdIter1, typename FwdIter2,
            HPX_CONCEPT_REQUIRES_(
                hpx::is_execution_policy<ExPolicy>::value &&
                hpx::traits::is_forward_iterator<FwdIter1>::value &&
                hpx::traits::is_forward_iterator<FwdIter2>::value
            )>
        // clang-format on
        friend typename parallel::util::detail::algorithm_result<ExPolicy,
            FwdIter2>::type
        tag_fallback_invoke(hpx::uninitialized_move_t, ExPolicy&& policy,
            FwdIter1 first, FwdIter1 last, FwdIter2 dest)
        {
            static_assert(hpx::traits::is_forward_iterator<FwdIter1>::value,
                "Requires at least forward iterator.");
            static_assert(hpx::traits::is_forward_iterator<FwdIter2>::value,
                "Requires at least forward iterator.");

            return parallel::util::get_second_element(
                hpx::parallel::v1::detail::uninitialized_move<
                    parallel::util::in_out_result<FwdIter1, FwdIter2>>()
                    .call(HPX_FORWARD(ExPolicy, policy), first, last, dest));
        }

    } uninitialized_move{};

    ///////////////////////////////////////////////////////////////////////////
    // DPO for hpx::uninitialized_move_n
    inline constexpr struct uninitialized_move_n_t final
      : hpx::detail::tag_parallel_algorithm<uninitialized_move_n_t>
    {
        // clang-format off
        template <typename InIter, typename Size,
            typename FwdIter,
            HPX_CONCEPT_REQUIRES_(
                hpx::traits::is_iterator<InIter>::value &&
                hpx::traits::is_forward_iterator<FwdIter>::value
            )>
        // clang-format on
        friend std::pair<InIter, FwdIter> tag_fallback_invoke(
            hpx::uninitialized_move_n_t, InIter first, Size count, FwdIter dest)
        {
            static_assert(hpx::traits::is_input_iterator<InIter>::value,
                "Required at least input iterator.");
            static_assert(hpx::traits::is_forward_iterator<FwdIter>::value,
                "Requires at least forward iterator.");

            // if count is representing a negative value, we do nothing
            if (hpx::parallel::v1::detail::is_negative(count))
            {
                return std::pair<InIter, FwdIter>(first, dest);
            }

            return parallel::util::get_pair(
                hpx::parallel::v1::detail::uninitialized_move_n<
                    parallel::util::in_out_result<InIter, FwdIter>>()
                    .call(
                        hpx::execution::seq, first, std::size_t(count), dest));
        }

        // clang-format off
        template <typename ExPolicy, typename FwdIter1, typename Size, typename FwdIter2,
            HPX_CONCEPT_REQUIRES_(
                hpx::is_execution_policy<ExPolicy>::value &&
                hpx::traits::is_forward_iterator<FwdIter1>::value &&
                hpx::traits::is_forward_iterator<FwdIter2>::value
            )>
        // clang-format on
        friend typename parallel::util::detail::algorithm_result<ExPolicy,
            std::pair<FwdIter1, FwdIter2>>::type
        tag_fallback_invoke(hpx::uninitialized_move_n_t, ExPolicy&& policy,
            FwdIter1 first, Size count, FwdIter2 dest)
        {
            static_assert(hpx::traits::is_forward_iterator<FwdIter1>::value,
                "Requires at least forward iterator.");
            static_assert(hpx::traits::is_forward_iterator<FwdIter2>::value,
                "Requires at least forward iterator.");

            // if count is representing a negative value, we do nothing
            if (hpx::parallel::v1::detail::is_negative(count))
            {
                return parallel::util::detail::algorithm_result<ExPolicy,
                    std::pair<FwdIter1, FwdIter2>>::get(std::pair<FwdIter1,
                    FwdIter2>(first, dest));
            }

            return parallel::util::get_pair(
                hpx::parallel::v1::detail::uninitialized_move_n<
                    parallel::util::in_out_result<FwdIter1, FwdIter2>>()
                    .call(HPX_FORWARD(ExPolicy, policy), first,
                        std::size_t(count), dest));
        }

    } uninitialized_move_n{};
}    // namespace hpx

#endif    // DOXYGEN
