#ifndef DISCRETE_CLIENTCONFIG_H
#define DISCRETE_CLIENTCONFIG_H

#include "yaml-cpp/yaml.h"
#include <string>

struct ClientConfig{
    int glMajorVersion;
    int glMinorVersion;
    int initialWindowHeight;
    int initialWindowWidth;
    std::string windowTitle;
};

namespace YAML {
template<>
struct convert<ClientConfig> {
  static Node encode(const ClientConfig& rhs) {
    Node node{};
    node["gl-major-version"] = rhs.glMajorVersion;
    node["gl-minor-version"] = rhs.glMinorVersion;
    node["initial-window-height"] = rhs.initialWindowHeight;
    node["initial-window-width"] = rhs.initialWindowWidth;
    node["window-title"] = rhs.windowTitle;
    return node;
  }

  static bool decode(const Node& node, ClientConfig& rhs) {
    rhs.windowTitle = node["window-title"].Scalar();
    rhs.initialWindowWidth = node["initial-window-width"].as<int>();
    rhs.initialWindowHeight = node["initial-window-height"].as<int>();
    rhs.glMinorVersion = node["gl-major-version"].as<int>();
    rhs.glMajorVersion = node["gl-minor-version"].as<int>();
    return true;
  }
};
}



#endif //DISCRETE_CLIENTCONFIG_H
