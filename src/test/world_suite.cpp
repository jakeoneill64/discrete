#include "gtest/gtest.h"
#include "engine/world.h"
#include <expected>

TEST(world_suite, load_world_from_file){
    auto world = load_world("world.dat");
    ASSERT_TRUE(world.has_value());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

