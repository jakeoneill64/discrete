#include "json.h"
#include <regex>
#include <string>

const std::regex JsonNode::KEY_REGEX{R"(^[a-zA-Z_][a-zA-Z0-9_]*(\.[a-zA-Z_][a-zA-Z0-9_]*)*$)"};
const std::regex JsonNode::KEY_SPLITTER_REGEX{R"(\.)"};

JsonNode::JsonNode(const char* cString) : JsonNode(std::string(cString)) {}
