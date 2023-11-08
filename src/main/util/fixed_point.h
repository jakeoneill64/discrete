//
// Created by Jake M O'Neill on 04/08/2023.
//

#ifndef DISCRETE_FIXED_POINT_H
#define DISCRETE_FIXED_POINT_H

#include <cinttypes>
#include <cfloat>
#include <type_traits>
#include <algorithm>

constexpr bool is_base_2(uint64_t number){
    return (number != 1) && ((number & (number - 1)) == 0);
}

constexpr int64_t pow(int64_t number, uint8_t index){
    int64_t current{number};
    for(uint8_t i{1}; i <= index; i++){
        current *= number;
    }
    return current;
}



// implementation details: https://en.wikipedia.org/wiki/Fixed-point_arithmetic
template <
        std::size_t size,
        uint64_t scaling_factor_inverse,
        bool is_signed = true,
        typename = typename std::enable_if_t<is_base_2(scaling_factor_inverse)>,
        typename UnderlyingIntegerType =
            typename std::conditional_t<(size == 16) && is_signed,
                std::int16_t,
            typename std::conditional_t<(size == 16) && !is_signed,
                std::uint16_t,
            typename std::conditional_t<(size == 32) && is_signed,
                std::int32_t,
            typename std::conditional_t<(size == 32) && !is_signed,
                std::uint32_t,
            typename std::conditional_t<(size == 64) && is_signed,
                std::int64_t,
            typename std::conditional_t<(size == 64) && !is_signed,
                std::uint64_t,
                void
            >>>>>
        >
>
class fixed{

public:

    // NOLINTBEGIN ====> clang-tidy complains about implicit conversions, which I want to enable.
    template <
            typename Number,
            typename std::enable_if_t<std::is_arithmetic_v<Number>,int > = 0>
    fixed(const Number& number){
        static_assert(
            std::is_signed_v<Number> ? is_signed : true,
            "cannot create unsigned fixed-point from signed arithmetic type"
        );
        data = number * scaling_factor_inverse;
    }

    fixed( const fixed & ) = default;

    template<
        typename Number,
        typename = typename std::enable_if_t<std::is_arithmetic_v<Number>>
    >
    operator Number() const
    {
        static_assert(
            std::is_unsigned_v<Number> ? !is_signed : true,
            "cannot convert signed fixed-point to an unsigned arithmetic type"
        );
        return
        static_cast<Number>(
            static_cast<long double>(data) /
            static_cast<UnderlyingIntegerType>(scaling_factor_inverse)
        )
        ;
    }

    // NOLINTEND

    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
    fixed<size, scaling_factor_inverse> operator+(Number operand) const
    {
        return (*this) + fixed<size, scaling_factor_inverse>{operand};
    }

    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
    fixed<size, scaling_factor_inverse> operator-(Number operand) const
    {
        return (*this) - fixed<size, scaling_factor_inverse>{operand};
    }

    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
    fixed<size, scaling_factor_inverse> operator*(Number operand) const
    {
        return (*this) * fixed<size, scaling_factor_inverse>{operand};
    }

    template < typename Number, typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
    fixed<size, scaling_factor_inverse> operator/(Number operand) const
    {
        return (*this) / fixed<size, scaling_factor_inverse>{operand};
    }

    template<size_t operand_size, uint64_t operand_scaling_factor>
    fixed<size, scaling_factor_inverse> operator+(const fixed<operand_size, operand_scaling_factor>& operand) const
    {
        fixed<std::max(size, operand_size), std::max(operand_scaling_factor, scaling_factor_inverse)> result{};
        result.data = this->data + operand.data;
        return result;
    }

    template<size_t operand_size, uint64_t operand_scaling_factor>
    fixed<size, scaling_factor_inverse> operator-(const fixed<operand_size, operand_scaling_factor>& operand) const
    {
        fixed<std::max(size, operand_size), std::max(operand_scaling_factor, scaling_factor_inverse)> result{};
        result.data = this->data - operand.data;
        return result;
    }

    template<size_t operand_size, uint64_t operand_scaling_factor>
    fixed<size, scaling_factor_inverse> operator*(const fixed<operand_size, operand_scaling_factor>& operand) const
    {
        fixed<std::max(size, operand_size), std::max(operand_scaling_factor, scaling_factor_inverse)> result{};
        result.data =
            static_cast<decltype(result.data)>(
                (this->data) *
                (static_cast<long double>(operand.data) / operand_scaling_factor)
            );
        return result;
    }

    template<size_t operand_size, uint64_t operand_scaling_factor>
    fixed<size, scaling_factor_inverse> operator/(const fixed<operand_size, operand_scaling_factor>& operand) const
    {
        fixed<std::max(size, operand_size), std::max(operand_scaling_factor, scaling_factor_inverse)> result{};
        result.data = static_cast<decltype(result.data)>(
                (this->data) /
                (static_cast<long double>(operand.data) / operand_scaling_factor)
        );
        return result;
    }

private:

    UnderlyingIntegerType data;
    fixed() = default;

};

using ufixed64_low = fixed<64, pow(2, 16), false>;
using ufixed32_low = fixed<32, pow(2, 8), false>;
using ufixed16_low = fixed<16, pow(2, 4), false>;
using ufixed64_mid = fixed<64, pow(2, 32), false>;
using ufixed32_mid = fixed<32, pow(2, 16), false>;
using ufixed16_mid = fixed<16, pow(2, 8), false>;
using ufixed64_high = fixed<64, pow(2, 48), false>;
using ufixed32_high = fixed<32, pow(2, 24), false>;
using ufixed16_high = fixed<16, pow(2, 12), false>;
using fixed64_low = fixed<64, pow(2, 16), true>;
using fixed32_low = fixed<32, pow(2, 8), true>;
using fixed16_low = fixed<16, pow(2, 4), true>;
using fixed64_mid = fixed<64, pow(2, 32), true>;
using fixed32_mid = fixed<32, pow(2, 16), true>;
using fixed16_mid = fixed<16, pow(2, 8), true>;
using fixed64_high = fixed<64, pow(2, 48), true>;
using fixed32_high = fixed<32, pow(2, 24), true>;
using fixed16_high = fixed<16, pow(2, 12), true>;



#endif //DISCRETE_FIXED_POINT_H