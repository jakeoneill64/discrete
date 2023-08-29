//
// Created by Jake M O'Neill on 04/08/2023.
//

#ifndef DISCRETE_FIXED_POINT_H
#define DISCRETE_FIXED_POINT_H

#include <cinttypes>
#include <cfloat>
#include <type_traits>
#include <algorithm>

//TODO should we limit conversion precision?
//TODO increment / decrement operator ?
template <std::size_t number>
struct is_base_2 : std::integral_constant<
        bool,
        true
        > {};

// implementation details: https://en.wikipedia.org/wiki/Fixed-point_arithmetic
template <
        std::size_t size,
        std::size_t scaling_factor_inverse,
        bool is_signed = true,
        typename = typename std::enable_if_t< is_base_2<scaling_factor_inverse>::value>,
        typename UnderlyingIntegerType =
            typename std::conditional_t<(size == 8) && is_signed,
                std::int8_t,
            typename std::conditional_t<(size == 8) && !is_signed,
                std::uint8_t,
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
            >>>>>>>
        >
>
class fixed{

public:

    // NOLINTBEGIN ====> clang-tidy complains about implicit conversions, which I want to enable.
    //TODO assert that the constructor arguments are compatible with the type arguments
    template <
            typename Number,
            typename std::enable_if_t<std::is_arithmetic_v<Number>,int > = 0>
    fixed(const Number& number){
        data = number * scaling_factor_inverse;
    }

    template<
            typename Number,
            typename = typename std::enable_if_t<std::is_arithmetic_v<Number>>>
    operator Number(){
        return Number
        {static_cast<Number>(data) / static_cast<UnderlyingIntegerType>(scaling_factor_inverse)};
    }

    // NOLINTEND

    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
    fixed<size, scaling_factor_inverse> operator+(Number operand){
        return (*this) + fixed<size, scaling_factor_inverse>{operand};
    }

    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
    fixed<size, scaling_factor_inverse> operator-(Number operand){
        return (*this) - fixed<size, scaling_factor_inverse>{operand};
    }

    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
    fixed<size, scaling_factor_inverse> operator*(Number operand){
        return (*this) * fixed<size, scaling_factor_inverse>{operand};
    }

    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
    fixed<size, scaling_factor_inverse> operator/(Number operand){
        return (*this) / fixed<size, scaling_factor_inverse>{operand};
    }


    // NB. we take the precision from the current object, not the operand
    template<size_t operand_size, size_t operand_scaling_factor>
    fixed<size, scaling_factor_inverse> operator+(const fixed<operand_size, operand_scaling_factor>& operand){
        fixed<std::max(size, operand_size), scaling_factor_inverse> result{};
        result.data = this->data + operand.data;
        return result;
    }

    template<size_t operand_size, size_t operand_scaling_factor>
    fixed<size, scaling_factor_inverse> operator-(const fixed<operand_size, operand_scaling_factor>& operand){
        fixed<std::max(size, operand_size), scaling_factor_inverse> result{};
        result.data = this->data - operand.data;
        return result;
    }

    template<size_t operand_size, size_t operand_scaling_factor>
    fixed<size, scaling_factor_inverse> operator*(const fixed<operand_size, operand_scaling_factor>& operand){
        fixed<std::max(size, operand_size), scaling_factor_inverse> result{};
        result.data = this->data * operand.data;
        return result;
    }

    template<size_t operand_size, size_t operand_scaling_factor>
    fixed<size, scaling_factor_inverse> operator/(const fixed<operand_size, operand_scaling_factor>& operand){
        fixed<std::max(size, operand_size), scaling_factor_inverse> result{};
        result.data = this->data / operand.data;
        return result;
    }



private:

    UnderlyingIntegerType data;
    fixed() = default;

};

#endif //DISCRETE_FIXED_POINT_H