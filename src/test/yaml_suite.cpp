#include "gtest/gtest.h"
#include "util/yaml.h"
#include "yaml-cpp/yaml.h"
#include "client/client.h"

extern unsigned char default_client_config[];
extern unsigned int default_client_config_len;

TEST(yaml_suite, serialise_generic_config) {
    YAML::Node node = YAML::Load(std::string{reinterpret_cast<char*>(default_client_config), default_client_config_len});
    auto serialisedClientConfig = node["graphics"].as<ClientConfig>();
    int x = 0;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}