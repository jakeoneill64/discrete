#ifndef JSON_H
#define JSON_H

#include "./types.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <expected>
#include <regex>
#include <ranges>
#include <tuple>
#include <algorithm>

class JsonNode;

using JsonArray = std::vector<JsonNode>;
using JsonObject = std::unordered_map<std::string, JsonNode>;

using JsonValueTypes = type_list<
    bool,
    int,
    double,
    std::string,
    std::shared_ptr<JsonArray>,
    std::shared_ptr<JsonObject>
>;

using JsonScalarTypes = type_list<
    bool,
    int,
    double,
    std::string
>;

template <typename T>
concept valid_json_type = contains_type<JsonValueTypes, T>::value;

template <typename T>
concept json_scalar_type = contains_type<JsonScalarTypes, T>::value;

template <typename T>
concept json_scalar_vector_type =
    requires {
    typename T::value_type;
    requires json_scalar_type<typename T::value_type>;
    requires std::same_as<T, std::vector<typename T::value_type>>;
    };

enum JsonNodeError
{
    NoSuchKey,
    IncorrectType,
    InvalidKey,
    FlattenedJsonFormatError,
    InternalError
};

// TODO const correctness needs to be considered, specifically looking at operator[]
class JsonNode
{

public:

    template <valid_json_type NodeType>
    explicit JsonNode(const NodeType& value): m_value{value} {}
    explicit JsonNode(const to_variant_t<JsonValueTypes>& value): m_value{value} {}
    explicit JsonNode(const char* cString);

    std::expected<JsonNode, JsonNodeError> operator[](const std::string& key){

        if (!std::regex_match(key, KEY_REGEX))
            return std::unexpected(InvalidKey);

        const std::vector<std::string> key_components{
            std::sregex_token_iterator(key.begin(), key.end(), KEY_SPLITTER_REGEX, -1),
            std::sregex_token_iterator()
        };

        return keyAccess(key_components, 0, *this);

    }

    template <valid_json_type NodeType>
    std::expected<NodeType, JsonNodeError> operator[](const char* key){
        return this->operator[]<NodeType>(std::string(key));
    }

    template <valid_json_type NodeType>
    [[nodiscard]]
    std::expected<NodeType, JsonNodeError> as() const{

        if (std::holds_alternative<NodeType>(m_value))
            return std::get<NodeType>(m_value);

        return std::unexpected{IncorrectType};
    }

    // convenience method for scalar lists.
    template <json_scalar_vector_type VectorType>
    [[nodiscard]]
    std::expected<VectorType, JsonNodeError> as() const
    {
        // the only case this method doesn't support is when the
        // wrapped value is a JsonObject. Scalars, we wrap in a vector
        // if the user wants a vector.
        if (std::holds_alternative<std::shared_ptr<JsonObject>>(m_value))
            return std::unexpected{IncorrectType};

        using ScalarType = typename VectorType::value_type;

        std::shared_ptr underlyingList{
            std::holds_alternative<std::shared_ptr<JsonArray>>(m_value) ?
            std::get<std::shared_ptr<JsonArray>>(m_value) :
            std::make_shared<JsonArray>(1, *this)
        };

        std::vector<ScalarType> transformedVector{};

        for (const auto& potentialScalar : *underlyingList){
            std::expected<ScalarType, JsonNodeError> scalar = potentialScalar.as<ScalarType>();
            if (!scalar)
                return std::unexpected{scalar.error()};
            transformedVector.push_back(*scalar);
        }

        return transformedVector;
    }

    /** Take something of the form [("hi.mummy", "value")] and
     *  transforms it into a JsonNode: {"hi": { "mummy": "value" } }
     *
     *  Json arrays are represented by values with duplicate keys and
     *  non-scalar list elements are not permitted.
     *
     *  Strings are the only scalar values supported. If you want other types,
     *  parse the strings.
     *
     * @param flattened - the flattened json document
     * @return the parsed Json document
     */
    static std::expected<JsonNode, JsonNodeError>
    unflatten(const std::vector<std::tuple<std::string, std::string>>& flattened)
    {
        // tuples need to be sorted so we can group them by key prefixes
        auto sortedTuples = flattened;
        std::ranges::sort(sortedTuples, [](const auto& a, const auto& b) {
            return std::get<0>(a) < std::get<0>(b);
        });

        std::vector<std::tuple<std::vector<std::string>, std::string>> valuesByKeyComponents =
            flattened |
            std::views::transform([](const auto& entry){
                const auto& [key, value] = entry;
                const auto keyComponents = std::vector<std::string>{
                        std::sregex_token_iterator(key.begin(), key.end(), KEY_SPLITTER_REGEX, -1),
                        std::sregex_token_iterator()
                };
                return std::make_tuple(
                    keyComponents,
                    value
                );
            }) |
            std::ranges::to<std::vector<std::tuple<std::vector<std::string>, std::string>>>();
        auto valuesByKeyComponentsView = std::span(valuesByKeyComponents);
        return unflatten(valuesByKeyComponentsView, 0)
            .transform([]( const auto& rootNode)
            {
            return JsonNode(rootNode);
            });
    }

    // NOLINTNEXTLINE to implement if I ever plan to use
    std::expected<std::shared_ptr<JsonNode>, JsonNodeError> parse(const std::string& json);

    template <valid_json_type Type>
    [[nodiscard]] std::expected<Type, JsonNodeError> operator()(Type x) const{
        return as<Type>(x);
    }

private:
    const static std::regex KEY_REGEX;
    const static std::regex KEY_SPLITTER_REGEX;

    to_variant_t<JsonValueTypes> m_value;

    // NOLINTNEXTLINE ignore recursion warnings.
    static std::expected<JsonNode, JsonNodeError> keyAccess(
        const std::vector<std::string>& components,
        unsigned int index,
        JsonNode& currentNode
    ){

        if (index >= components.size())
            return currentNode;

        if (!std::holds_alternative<std::shared_ptr<JsonObject>>(currentNode.m_value))
            return std::unexpected(InvalidKey);

        const auto& currentObject{std::get<std::shared_ptr<JsonObject>>(currentNode.m_value)};
        const std::string& currentKey {components[index]};

        const auto nextNode{currentObject->find(currentKey)};

        if (nextNode == currentObject->end())
            return std::unexpected(InvalidKey);

        return keyAccess(components, ++index, nextNode->second);

    }

    // NOLINTNEXTLINE don't give me warnings about recursion
    static std::expected<JsonNode, JsonNodeError> unflatten(
        std::span<std::tuple<std::vector<std::string>, std::string>>& valuesByKeyComponents,
        unsigned int keyComponentIndex
    )
    {
        if (valuesByKeyComponents.empty())
            return std::unexpected{InternalError};

        // scalar base case
        if (valuesByKeyComponents.size() == 1 && std::get<0>(valuesByKeyComponents[0]).size() == keyComponentIndex)
            return JsonNode{std::get<1>(valuesByKeyComponents[0])};

        // use this to validate the non-scalar data and reference if creating object nodes
        std::vector<unsigned int> uniqueComponentIndicies{0};
        for (int index = 1; index < valuesByKeyComponents.size(); index++){

            const auto& currentKeyComponents = std::get<0>(valuesByKeyComponents[index]);
            // NOLINTNEXTLINE don't suggest init-if here, I like to be able to read my code
            const auto& previousKeyComponents = std::get<0>(valuesByKeyComponents[index - 1]);
            if (
                !(
                    currentKeyComponents.empty() && previousKeyComponents.empty() ||
                    currentKeyComponents[keyComponentIndex] == previousKeyComponents[keyComponentIndex]
                )
            )
                uniqueComponentIndicies.emplace_back(index);
        }

        // list of non-scalars not supported
        if (std::get<0>(valuesByKeyComponents[0]).empty() && uniqueComponentIndicies.size() > 1)
            return std::unexpected{FlattenedJsonFormatError};

        // array case
        if (std::get<0>(valuesByKeyComponents[0]).empty()){
            const auto arrayNode{std::shared_ptr<JsonArray>()};
            for (const auto& [_, value] : valuesByKeyComponents){
                arrayNode->emplace_back(value);
            }
            return JsonNode{arrayNode};
        }

        // edge case with a single object kv pair
        if (uniqueComponentIndicies.size() <= 1)
            uniqueComponentIndicies.emplace_back(1);

        // object case
        const auto jsonObject = std::make_shared<JsonObject>();
        for (unsigned int index = 1; index < uniqueComponentIndicies.size(); index++){

            auto keyView =
                std::span{
                    valuesByKeyComponents.data() + uniqueComponentIndicies[index - 1],
                    uniqueComponentIndicies[index] - uniqueComponentIndicies[index - 1]
                };

            const std::string currentKeyComponent {std::get<0>(valuesByKeyComponents[uniqueComponentIndicies[index -1]])[keyComponentIndex]};

            auto value = unflatten(keyView, keyComponentIndex + 1);
            if (!value)
                return value;

            jsonObject->emplace(currentKeyComponent, JsonNode(*value));

        }

        return JsonNode{jsonObject};
    }



};

// special case, if we want a vector / json array but the node is a scalar
// we will nicely wrap it for the user
template <>
inline std::expected<std::shared_ptr<JsonArray>, JsonNodeError> JsonNode::as() const{
    const bool isNonScalar =
            std::holds_alternative<std::shared_ptr<JsonObject>>(m_value) ||
            std::holds_alternative<std::shared_ptr<JsonArray>>(m_value);

    if (!isNonScalar)
        return std::make_shared<JsonArray>(
            JsonArray{ JsonNode{m_value} }
        );

    return std::unexpected{IncorrectType};
}

#endif //JSON_H
