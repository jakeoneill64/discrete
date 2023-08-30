#include <gtest/gtest.h>
#include "util/fixed_point.h"
#include <cinttypes>

// NB. all other fixed_point tests depend on conversions / constructors working correctly
// so if fixed_point_conversions fails, others will likely fail too.
TEST(fixed_point_suite, fixed_point_conversions) {
    fixed64_low fromUnsignedInteger{33};
    fixed8_low fromUnsignedFloat{3.5f};
    fixed64_low fromInteger{-33};
    fixed32_high fromFloat{-33.5f};
    int64_t unsignedIntegerResult = fromUnsignedInteger;
    int8_t signedIntegerResult = fromInteger;
    double unsignedFloatResult = fromUnsignedFloat;
    float signedFloatResult = fromFloat;
    ASSERT_EQ(signedFloatResult, -33.5);
    ASSERT_EQ(unsignedIntegerResult, 33);
    ASSERT_EQ(signedIntegerResult, -33);
    ASSERT_EQ(unsignedFloatResult, 3.5);

}

TEST(fixed_point_suite, fixed_point_addition) {
    fixed64_low fromUnsignedInteger{33};
    fixed8_low fromUnsignedFloat{3.5f};
    fixed64_low fromInteger{-33};
    fixed32_high fromFloat{-33.5f};

    fixed64_low unsignedIntegerPlusSignedDouble = fromUnsignedInteger + (-34.5f);
    fixed8_low unsignedFloatPlusSignedInteger = fromUnsignedFloat + (-1);
    fixed64_low signedIntegerPlusUnsignedDouble = fromInteger + 22.5;
    fixed32_high signedFloatPlusUnsignedInteger = fromFloat + 0.5f;

    int64_t unsignedIntegerResult = unsignedIntegerPlusSignedDouble;
    double floatAndIntegerResult = unsignedFloatPlusSignedInteger;
    double unsignedFloatResult = signedIntegerPlusUnsignedDouble;
    float signedFloatResult = signedFloatPlusUnsignedInteger;

    ASSERT_EQ(signedFloatResult, -33);
    ASSERT_EQ(unsignedIntegerResult, -1);
    ASSERT_EQ(floatAndIntegerResult, 2.5);
    ASSERT_EQ(unsignedFloatResult, -10.5);
}

TEST(fixed_point_suite, fixed_point_subtraction) {

}

TEST(fixed_point_suite, fixed_point_multiplication) {

}

TEST(fixed_point_suite, fixed_point_division) {

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}