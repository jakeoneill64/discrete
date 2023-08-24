//
// Created by Jake M O'Neill on 04/08/2023.
//

#ifndef DISCRETE_FIXED_POINT_H
#define DISCRETE_FIXED_POINT_H

#include <cinttypes>
#include <type_traits>

//TODO verify these template args
//TODO provide specialisations
//TODO how about fixed_point with different sizes?
template <std::size_t size, u_int64_t scaling_factor_inverse>
class fixed{

public:

    using UInt = typename std::conditional<(size == 8), std::uint8_t,
            typename std::conditional<(size == 16), std::uint16_t,
                    typename std::conditional<(size == 32), std::uint32_t,
                            typename std::conditional<(size == 64), std::uint64_t, //TODO if the last case is not true, we need to create a malformed type
                                    void>::type>::type>::type>::type;

    template <typename T>
    // NOLINTNEXTLINE
    fixed(const T& number, typename std::enable_if_t<std::is_floating_point_v<T>, int > = 0){
        data = number * scaling_factor_inverse;
    }

    template <typename T>
    // NOLINTNEXTLINE
    fixed(const T& number, typename std::enable_if_t<std::is_integral_v<T>, int> = 0){
        data = number << logBase2(scaling_factor_inverse);
    }

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
