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

constexpr uint64_t power(uint64_t number, uint8_t index){
    uint64_t current{number};
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

friend struct std::hash<fixed<size, scaling_factor_inverse, is_signed>>;

public:

    template <
            typename Number,
            typename std::enable_if_t<std::is_arithmetic_v<Number> , int> = 0>
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


private:

    UnderlyingIntegerType data;
    fixed() = default;

};

template<std::size_t Size, uint64_t ScalingFactorInverse, bool Signed>
struct std::hash<fixed<Size, ScalingFactorInverse, Signed>>{
        std::size_t operator()(::fixed<Size, ScalingFactorInverse, Signed> number) const {
        static std::hash<decltype(number.data)> hasher{};
        return hasher(number.data);
    }
};




#endif //DISCRETE_FIXED_POINT_H