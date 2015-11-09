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

#ifndef VARIANT_H_43A23194_853D_11E5_87E0_2BF3A5F1CA9B
#define VARIANT_H_43A23194_853D_11E5_87E0_2BF3A5F1CA9B

#include <typeinfo>
#include "variant_traits.h"
#include "variant_meta.h"


namespace detail {

template<bool B, typename T>
struct ostream_printer;

template<typename V, typename... Ts>
struct cast_helper;

template<typename... Ts>
struct variant_helper;

}

template<typename... Ts>
class Variant : public detail::cast_helper<Variant<Ts...>, Ts...> {
public:
    Variant() : type(nullptr) {
    }

    Variant(const Variant& that) : type(that.type) {
        detail::variant_helper<Ts...>::copy(&storage, &that.storage, type);
    }

    Variant(Variant&& that) : type(that.type) {
        detail::variant_helper<Ts...>::move(&storage, &that.storage, type);
    }

    template<typename Arg, typename = typename
            std::enable_if<meta::list_contains<meta::list<Ts...>, typename std::decay<Arg>::type>::value>::type>
    Variant(Arg&& arg) {
        using type_t = typename std::decay<Arg>::type;
        new (&storage) type_t(std::forward<Arg>(arg));
        type = &typeid(type_t);
    }

    template<typename... Args>
    Variant(Args&&... args) {
        using T = typename meta::list_find_if<meta::list<Ts...>, std::is_constructible, Args&&...>::type;
        new (&storage) T(std::forward<Args>(args)...);
        type = &typeid(T);
    }

    ~Variant() {
        detail::variant_helper<Ts...>::destroy(&storage, type);
    }

    template<typename T>
    T& get() {
        static_assert(meta::list_contains<meta::list<Ts...>, T>::value == true,
                "variant does not contain a type");

        if (typeid(T) == *type) {
            return *reinterpret_cast<T*>(&storage);
        } else {
            throw std::bad_cast();
        }
    }

    template<typename T>
    const T& get() const {
        static_assert(meta::list_contains<meta::list<Ts...>, T>::value == true,
                "variant does not contain a type");

        if (typeid(T) == *type) {
            return *reinterpret_cast<const T*>(&storage);
        } else {
            throw std::bad_cast();
        }
    }

    template<typename T, typename... Args>
    void set(Args&&... args) {
        static_assert(meta::list_contains<meta::list<Ts...>, T>::value == true,
                        "variant does not contain a type");

        detail::variant_helper<Ts...>::destroy(&storage, type);
        new (&storage) T(std::forward<Args>(args)...);
        type = &typeid(T);
    }

    template<typename... Args>
    Variant& operator=(Args&&... args) {
        detail::variant_helper<Ts...>::destroy(&storage, type);
        using T = typename meta::list_find_if<meta::list<Ts...>, std::is_constructible, Args&&...>::type;
        new (&storage) T(std::forward<Args>(args)...);
        type = &typeid(T);
        return *this;
    }

    friend inline std::ostream& operator<<(std::ostream& out, const Variant& v) {
        if (v.type != nullptr) {
            detail::variant_helper<Ts...>::print(out, &v.storage, v.type);
        } else {
            out << "(null)";
        }
        return out;
    }

protected:
    void destroy() {
        if (type != nullptr) {

            type = nullptr;
        }
    }

    static constexpr auto size = meta::max<sizeof(Ts)...>::value;
    static constexpr auto align = meta::max<sizeof(void*), alignof(Ts)...>::value;

    using StorageType = typename std::aligned_storage<size, align>::type;
    StorageType storage;
    const std::type_info *type;

    template<typename V, typename... Ns> friend
    class detail::cast_helper;
};


namespace detail {

template<typename T, typename ... Ts>
struct variant_helper<T, Ts...> {

    static void copy(void *dst, const void *src, const std::type_info *type) {
        if (*type == typeid(T)) {
            new (dst) T(*reinterpret_cast<const T*>(src));
        } else {
            variant_helper<Ts...>::copy(dst, src, type);
        }
    }

    static void move(void *dst, void *src, const std::type_info *type) {
        if (*type == typeid(T)) {
            new (dst) T(std::move(*reinterpret_cast<T*>(src)));
        } else {
            variant_helper<Ts...>::move(dst, src, type);
        }
    }

    static void destroy(void *storage, const std::type_info*& type) {
        if (type != nullptr) {
            if (*type == typeid(T)) {
                reinterpret_cast<T*>(storage)->~T();
                type = nullptr;
            } else {
                variant_helper<Ts...>::destroy(storage, type);
            }
        }
    }

    static void print(std::ostream& out, const void *storage, const std::type_info *type) {
        if (*type == typeid(T)) {
            const T& obj = *reinterpret_cast<const T*>(storage);
            detail::ostream_printer<trait::is_ostreamable<T>::value, T>::print(out, obj);
        } else {
            variant_helper<Ts...>::print(out, storage, type);
        }
    }
};

template<>
struct variant_helper<> {
    static void copy(void *dst, const void *src, const std::type_info *type) { }
    static void move(void *dst, void *src, const std::type_info *type) { }
    static void destroy(void *storage, const std::type_info *type) { }
    static void print(std::ostream& out, const void *storage, const std::type_info*& type) { }
};

template<typename V, typename T, typename... Ts>
struct cast_helper<V, T, Ts...> : public cast_helper<V, Ts...> {
    operator T() {
        if (*this->getThis()->type == typeid(T)) {
            return *reinterpret_cast<T*>(&this->getThis()->storage);
        } else {
            throw std::bad_cast();
        }
    }
};

template<typename V>
struct cast_helper<V> {
protected:
    V* getThis() {
        return reinterpret_cast<V*>(this);
    }
};

template<typename T>
struct ostream_printer<true, T> {
    static void print(std::ostream& out, const T& t) {
        out << t;
    }
};

template<typename T>
struct ostream_printer<false, T> {
    static void print(std::ostream& out, const T& t) {
        out << "(unknown)";
    }
};

}

#endif /* VARIANT_H_43A23194_853D_11E5_87E0_2BF3A5F1CA9B */
