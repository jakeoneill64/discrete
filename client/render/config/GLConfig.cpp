//
// Created by Jake M O'Neill on 07/08/2022.
//

#include "GLConfig.h"

uint32_t GLConfig::getMajorVersion() {
    return majorVersion;
}

uint32_t GLConfig::getMinorVersion() {
    return minorVersion;
}

uint32_t GLConfig::getProfile() {
    return profile;
}

uint32_t GLConfig::getForwardCompatible() {
    return forwardCompatible;
}
