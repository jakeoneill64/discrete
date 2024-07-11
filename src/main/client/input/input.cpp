#include "input.h"
#include "default-input-config.dat"
#include "yaml-cpp/yaml.h"

std::unordered_map<char, InputAction> load_default_input_mappings(std::filesystem::path mappingFile){
    // TODO get xxd to use the filename component (not the path)
    YAML::Node node = YAML::Load()
}