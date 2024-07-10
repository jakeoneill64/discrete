#include "gtest/gtest.h"
#include "event/EventManager.h"

TEST(event_suite, test_integer_event){
    EventManager eventManager{};
    int integerToSet{};
    eventManager.subscribeEvent<int>([&integerToSet](int event){integerToSet = event;});
    eventManager.publishEvent<int>(52);
    ASSERT_EQ(integerToSet, 52);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}