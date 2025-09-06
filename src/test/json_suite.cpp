#include <gtest/gtest.h>

#include "util/json.h"

#include <memory>
#include <tuple>
#include <vector>
#include <string>


struct UnflattenCase {
    std::vector<std::tuple<std::string, std::string>> input;
    std::function<void(JsonNode)> validate;
};

class UnflattenParamTest : public ::testing::TestWithParam<UnflattenCase> {};

struct UnflattenErrorCase {
    std::vector<std::tuple<std::string, std::string>> input;
    JsonNodeError expectedError;
};

class UnflattenErrorTest : public ::testing::TestWithParam<UnflattenErrorCase> {};

TEST_P(UnflattenParamTest, HandlesFlattenedInputCorrectly) {
    const auto& [input, validate] = GetParam();
    const auto result = JsonNode::unflatten(input);
    ASSERT_TRUE(result.has_value());
    validate(result.value());
}

// TODO should segregate these properly.
INSTANTIATE_TEST_SUITE_P(
    ValidJsonTests,
    UnflattenParamTest,
    ::testing::Values(
        // single nested entry, single string scalar retrieval
        UnflattenCase{
            {{"vulkan.device_extensions", "VK_KHR_swapchain"}},
            [](JsonNode root) {
                const auto& expectedStringResult =
                    root["vulkan.device_extensions"]
                        .and_then([](const JsonNode& deviceExtensionsNode){
                                return deviceExtensionsNode.as<std::string>();
                            }
                        );
                ASSERT_TRUE(expectedStringResult.has_value());
                ASSERT_EQ(expectedStringResult.value(), "VK_KHR_swapchain");
            }
        },
        UnflattenCase{
            {{"vulkan.device_extensions", "VK_KHR_swapchain"}},
            [](JsonNode root) {
                const auto& expectedStringResult =
                    root["vulkan.device_extensions"]
                        .and_then([](const JsonNode& deviceExtensionsNode){
                                return deviceExtensionsNode.as<std::vector<std::string>>();
                            }
                        );
                    ASSERT_TRUE(expectedStringResult.has_value());
                    ASSERT_EQ(expectedStringResult.value(), std::vector<std::string>{"VK_KHR_swapchain"});

            }
        }
        // UnflattenCase{
        //     {{"vulkan.device_extensions", "VK_KHR_swapchain"}},
        //     [](JsonNode root) {
        //         const std::expected<JsonNode, JsonNodeError> device_extensions{root["vulkan.device_extensions"]};
        //         ASSERT_TRUE(device_extensions.has_value());
        //         const JsonNode& deviceExtensionNode = device_extensions.value();
        //         const auto& singletonVector = deviceExtensionNode.as<std::shared_ptr<JsonArray>>();
        //         ASSERT_EQ(*singletonVector, JsonArray{JsonNode{"VK_KHR_swapchain"}});
        //     }
        // }
        )
);

// INSTANTIATE_TEST_SUITE_P(
//     InvalidJsonTests,
//     UnflattenErrorTest,
//     ::testing::Values(
//         UnflattenErrorCase{
//             {{"foo.bar", "value1"}, {"foo.bar", "value2"}},
//             JsonNodeError::DuplicateKey
//         },
//         UnflattenErrorCase{
//             {{"", "value"}},
//             JsonNodeError::InvalidKey
//         },
//         UnflattenErrorCase{
//             {{"a", "value1"}, {"a.b", "value2"}},
//             JsonNodeError::DuplicateKey
//         }
//     )
// );




