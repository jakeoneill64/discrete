#include <gtest/gtest.h>
#include "util/fixed_point.h"

TEST(FixedPointSuite, FloatConversions) {
    fixed<64, 32> floater{32.5};
    int x = 0;
    //long double andBack = floater;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}