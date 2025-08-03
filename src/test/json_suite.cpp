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

// TEST_P(UnflattenErrorTest, HandlesInvalidInputs) {
//     const auto& [input, expectedError] = GetParam();
//     auto result = JsonNode::unflatten(input);
//     ASSERT_FALSE(result.has_value());
//     ASSERT_EQ(result.error(), expectedError);
// }

INSTANTIATE_TEST_SUITE_P(
    ValidJsonTests,
    UnflattenParamTest,
    ::testing::Values(
        UnflattenCase{
            {{"vulkan.device_extensions", "VK_KHR_swapchain"}},
            [](JsonNode root) {
                const std::expected<JsonNode, JsonNodeError> device_extensions{root["vulkan.device_extensions"]};
                ASSERT_TRUE(device_extensions.has_value());
                JsonNode x = device_extensions.value();
                const auto& c = x.as<std::string>();
                ASSERT_EQ(c, "VK_KHR_swapchain");
            }
        }
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




