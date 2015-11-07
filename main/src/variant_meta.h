/**
 * Copyright (c) 2015, Mateusz Kramarczyk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of variant-cpp nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VARIANT_META_H_FF9D86D2_853D_11E5_918C_B3E38B9DEAE0
#define VARIANT_META_H_FF9D86D2_853D_11E5_918C_B3E38B9DEAE0

#include <type_traits>

namespace meta {
template<typename...> struct list;
template<typename L>  struct list_head;
template<typename L>  struct list_tail;
}

namespace meta {
namespace detail {

template<typename L, typename T>
struct list_contains_impl;

template<typename T>
struct list_contains_impl<list<>, T> {
    static constexpr bool value = false;
};

template<typename L, typename T>
struct list_contains_impl {
    using head = typename list_head<L>::type;
    using tail = typename list_tail<L>::type;

    static constexpr bool value = std::is_same<head, T>::value || list_contains_impl<tail, T>::value;
};

struct list_find_if_impl_end;

template<typename L, template <typename, typename... > class Pred, typename... Args>
struct list_find_if_impl {
    using head = typename list_head<L>::type;
    using tail = typename list_tail<L>::type;
    using type = typename std::conditional<Pred<head, Args...>::value, head, typename list_find_if_impl<tail, Pred, Args...>::type>::type;
};

template<template <typename, typename... > class Pred, typename... Args>
struct list_find_if_impl<list<>, Pred, Args...> {
    using type = list_find_if_impl_end;
};

template<typename L, template <typename, typename... > class Pred, typename... Args>
struct list_find_if_impl2 {
    using e = typename list_find_if_impl<L, Pred, Args...>::type;
    using type = typename std::enable_if<!std::is_same<e, list_find_if_impl_end>::value, e>::type;
};

template<std::size_t...>
struct max_impl;

template<>
struct max_impl<> {
    static constexpr std::size_t value = 0;
};

template<std::size_t car, std::size_t... cdr>
struct max_impl<car, cdr...> {
    static constexpr std::size_t cdr_max = max_impl<cdr...>::value;
    static constexpr std::size_t value = car < cdr_max ? cdr_max : car;
};

}

template<typename... Ts>
struct list {
};

template<typename L>
struct list_size;

template<typename... Ts>
struct list_size<list<Ts...>> : std::integral_constant<decltype(sizeof...(Ts)), sizeof...(Ts)> {
};

template<typename L>
struct list_head;

template<typename H, typename... T>
struct list_head<list<H, T...>> {
    using type = H;
};

template<typename L>
struct list_tail;

template<>
struct list_tail<list<>> {
    using type = list<>;
};

template<typename H, typename... T>
struct list_tail<list<H, T...>> {
    using type = list<T...>;
};

template<typename L, typename T>
struct list_contains : std::integral_constant<bool, detail::list_contains_impl<L, T>::value> {
};

template<typename L, template <typename, typename... > class Pred, typename... Args>
struct list_find_if {
    using type = typename detail::list_find_if_impl2<L, Pred, Args...>::type;
};

template<std::size_t ... vs>
struct max : std::integral_constant<std::size_t, detail::max_impl<vs...>::value> {
};


}

#endif /* VARIANT_META_H_FF9D86D2_853D_11E5_918C_B3E38B9DEAE0 */
