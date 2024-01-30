//
// Created by Jake M O'Neill on 27/01/2024.
//

#include "gtest/gtest.h"
#include "network/datagram.h"

TEST(datagram_suite, datagram_send){
    int t{1};
    DatagramSocket sock{false};
    sock.send({const_cast<char *>("hello world"), sizeof("hello world")}, "172.0.0.1", 1637);
    int x{0};
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

