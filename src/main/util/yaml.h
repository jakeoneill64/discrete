#ifndef DISCRETE_YAML_H
#define DISCRETE_YAML_H

#include "yaml-cpp/yaml.h"

namespace YAML {
template <typename Aggregate>
struct convert {
  static Node encode(const Aggregate& rhs) {
    return Aggregate{};
  }

  static bool decode(const Node& node, Aggregate& rhs) {
    return true;
  }
};
}

#endif //DISCRETE_YAML_H
