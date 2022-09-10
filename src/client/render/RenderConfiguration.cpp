//
// Created by Jake M O'Neill on 09/08/2022.
//

#include "RenderConfiguration.h"

#include <utility>

Render::RenderConfiguration::RenderConfiguration(
        int glMajorVersion,
        int glMinorVersion,
        int glForwardCompatible,
        int windowStartWidth,
        int windowStartHeight,
        std::string windowName,
        int glProfile
        ):
glMajorVersion(glMajorVersion),
glMinorVersion(glMinorVersion),
glForwardCompatible(glForwardCompatible),
windowStartHeight(windowStartHeight),
windowStartWidth(windowStartWidth),
windowName(std::move(windowName)),
glProfile(glProfile)
{}

