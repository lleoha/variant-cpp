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

#ifndef VARIANT_TRAITS_H_F2BB1B8E_853B_11E5_B168_4B40D7CD81FD
#define VARIANT_TRAITS_H_F2BB1B8E_853B_11E5_B168_4B40D7CD81FD

#include <ostream>
#include <type_traits>


namespace trait {
namespace detail {

struct is_ostreamable_impl {
    template<typename T, typename = decltype(std::declval<std::ostream&>() << std::declval<T>())>
    static std::true_type test(int);

    template<typename>
    static std::false_type test(...);
};

}

template<typename T>
struct is_ostreamable : decltype(detail::is_ostreamable_impl::test<T>(0))::type {
};

}

#endif /* VARIANT_TRAITS_H_F2BB1B8E_853B_11E5_B168_4B40D7CD81FD */
