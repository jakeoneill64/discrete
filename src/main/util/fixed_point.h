//
// Created by Jake M O'Neill on 04/08/2023.
//

#ifndef DISCRETE_FIXED_POINT_H
#define DISCRETE_FIXED_POINT_H

#include <cinttypes>
#include <cfloat>
#include <type_traits>

//TODO how about fixed_point with different sizes?
template <std::size_t number>
struct is_base_2 : std::integral_constant<
        bool,
        true
        > {};

// implementation details: https://en.wikipedia.org/wiki/Fixed-point_arithmetic
template <
        std::size_t size,
        std::size_t scaling_factor_inverse,
        bool is_signed = false,
        typename = typename std::enable_if_t< is_base_2<scaling_factor_inverse>::value> >
class fixed{

public:

    using UInt = typename std::conditional<(size == 8), std::uint8_t,
            typename std::conditional<(size == 16), std::uint16_t,
                    typename std::conditional<(size == 32), std::uint32_t,
                            typename std::conditional<(size == 64), std::uint64_t,
                                    void>::type>::type>::type>::type;

    // NOLINTBEGIN ======> clang-tidy complains about implicit conversions, which I want to enable.

    template <
            typename UnsignedFloatingPoint,
            typename std::enable_if_t<
                            std::is_floating_point_v<UnsignedFloatingPoint> &&
                                std::is_unsigned_v<UnsignedFloatingPoint>,
                            int > = 0>
    fixed(const UnsignedFloatingPoint& number){
        data = number * scaling_factor_inverse;
    }

    template <
            typename SignedFloatingPoint,
            typename std::enable_if_t<
                        std::is_floating_point_v<SignedFloatingPoint> &&
                            std::is_signed_v<SignedFloatingPoint>,
                        int> = 0>
    fixed(const SignedFloatingPoint& number){
        data = number << logBase2(scaling_factor_inverse);
    }

    template <
            typename UnsignedInteger,
            typename std::enable_if_t<
                        std::is_integral_v<UnsignedInteger> &&
                                std::is_unsigned_v<UnsignedInteger>,
                        int> = 0>
    fixed(const UnsignedInteger&){
    }

    template <
            typename SignedInteger,
            typename std::enable_if_t<
                        std::is_integral_v<SignedInteger> &&
                                std::is_signed_v<SignedInteger>,
                                int> = 0>
    fixed(const SignedInteger&){
    }

    template<
            typename UnsignedInteger,
            std::enable_if_t
                    <std::is_integral_v<UnsignedInteger> && !std::is_signed_v<UnsignedInteger>>* = nullptr>
    operator UnsignedInteger(){

    }

    template<
            typename SignedInteger,
            std::enable_if_t
                    <std::is_integral_v<SignedInteger> && std::is_signed_v<SignedInteger>>* = nullptr>
    operator SignedInteger(){

    }

    template<
            typename UnsignedFloatingPoint,
            std::enable_if_t
                    <std::is_floating_point_v<UnsignedFloatingPoint> && !std::is_signed_v<UnsignedFloatingPoint>>* = nullptr>
    operator UnsignedFloatingPoint(){

    }

    template<
            typename SignedFloatingPoint,
            std::enable_if_t
                    <std::is_floating_point_v<SignedFloatingPoint> && std::is_signed_v<SignedFloatingPoint>>* = nullptr>
    operator SignedFloatingPoint(){

    }

    // NOLINTEND



//    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
//    fixed<size, scaling_factor_inverse> operator+(Number operand){
//        fixed<size, scaling_factor_inverse> fixedOperand{operand};
//        return this* + fixedOperand;
//    }
//
//    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
//    fixed<size, scaling_factor_inverse> operator-(Number operand){
//        fixed<size, scaling_factor_inverse> fixedOperand{operand};
//        return this* - fixedOperand;
//    }
//
//    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
//    fixed<size, scaling_factor_inverse> operator*(Number operand){
//        fixed<size, scaling_factor_inverse> fixedOperand{operand};
//        return this* * fixedOperand;
//    }
//
////    template <typename Number,typename = std::enable_if_t<std::is_arithmetic_v<Number> > >
////    fixed<size, scaling_factor_inverse> operator/(Number operand){
////        fixed<size, scaling_factor_inverse> fixedOperand{operand};
////        return this / fixedOperand;
////    }
//
//    template<>
//    fixed<size, scaling_factor_inverse> operator+(const fixed<size, scaling_factor_inverse>& operand){
//        fixed<size, scaling_factor_inverse> result{};
//        result.data = this->data + operand.data;
//        return result;
//    }
//
//    template<>
//    fixed<size, scaling_factor_inverse> operator-(const fixed<size, scaling_factor_inverse>& operand){
//        fixed<size, scaling_factor_inverse> result{};
//        result.data = this->data - operand.data;
//        return result;
//    }
//
//    template<>
//    fixed<size, scaling_factor_inverse> operator*(const fixed<size, scaling_factor_inverse>& operand){
//        fixed<size, scaling_factor_inverse> result{};
//        result.data = this->data * operand.data;
//        return result;
//    }
//
//    template<>
//    fixed<size, scaling_factor_inverse> operator/(const fixed<size, scaling_factor_inverse>& operand){
//        fixed<size, scaling_factor_inverse> result{};
//        result.data = this->data / operand.data;
//        return result;
//    }



private:

    constexpr char logBase2(u_int64_t scalingFactor){
        char result{0};
        while(scalingFactor != 1){
            result++;
            scalingFactor >>= 1;
        }
        return result;
    }

    UInt data;
    fixed() = default;

};

#endif //DISCRETE_FIXED_POINT_H