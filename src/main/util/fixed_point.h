#ifndef DISCRETE_FIXED_POINT_H
#define DISCRETE_FIXED_POINT_H

#include <cinttypes>
#include <cfloat>
#include <type_traits>
#include <algorithm>

constexpr bool is_base_2(uint64_t number){
    return (number != 1) && ((number & (number - 1)) == 0);
}

constexpr uint64_t power(uint64_t number, uint8_t index){
    uint64_t current{number};
    for(uint8_t i{2}; i <= index; i++){
        current *= number;
    }
    return current;
}

// implementation details: https://en.wikipedia.org/wiki/Fixed-point_arithmetic
template <
        std::size_t size,
        uint64_t scaling_factor_inverse,
        typename = typename std::enable_if_t<is_base_2(scaling_factor_inverse)>,
        typename UnderlyingIntegerType =
            typename std::conditional_t<(size == 16),
                std::int16_t,
            typename std::conditional_t<(size == 32),
                std::int32_t,
            typename std::conditional_t<(size == 64),
                std::int64_t,
                void
            >>
        >
>
class fixed{

friend struct std::hash<fixed<size, scaling_factor_inverse>>;

public:

    //NOLINTBEGIN
    template <
            typename Number,
            typename std::enable_if_t<std::is_arithmetic_v<Number> , int> = 0>
    fixed(const Number& number){
        data = number * scaling_factor_inverse;
    }

    fixed( const fixed & ) = default;
    fixed( fixed && ) = default;
    fixed() = default;

    template<
        typename Number,
        typename = typename std::enable_if_t<std::is_arithmetic_v<Number>>
    >
    operator Number() const
    {
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

    // signed (all floating point types are signed), so we need assume this is signed too
    UnderlyingIntegerType data;

};

using fixed64_low = fixed<64, power(2, 15)>;
using fixed32_low = fixed<32, power(2, 7)>;
using fixed16_low = fixed<16, power(2, 3)>;
using fixed64_mid = fixed<64, power(2, 31)>;
using fixed32_mid = fixed<32, power(2, 17)>;
using fixed16_mid = fixed<16, power(2, 7)>;
using fixed64_high = fixed<64, power(2, 47)>;
using fixed32_high = fixed<32, power(2, 23)>;
using fixed16_high = fixed<16, power(2, 11)>;

template<std::size_t S, uint64_t T>
struct std::hash<fixed<S, T>>{
    std::size_t operator()(const ::fixed<S, T>& number){
        static std::hash<decltype(number.data)> hasher{};
        return hasher(number.data);
    }
};






#endif //DISCRETE_FIXED_POINT_H