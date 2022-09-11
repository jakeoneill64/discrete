//
// Created by Jake M O'Neill on 06/08/2022.
//

#ifndef MRONEILLSCUBES_RENDERCONFIGURATION_H
#define MRONEILLSCUBES_RENDERCONFIGURATION_H

#include <cinttypes>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <string>

namespace Render {

    class RenderConfiguration {

    public :

        RenderConfiguration(int& glMajorVersion, int glMinorVersion, int glForwardCompatible,
                            int windowStartWidth, int windowStartHeight, std::string windowName, int glProfile);

        const int glMajorVersion;
        const int glMinorVersion;
        const int glForwardCompatible;
        const int glProfile;
        const int windowStartWidth;
        const int windowStartHeight;
        const std::string windowName;


    };

}

#endif //MRONEILLSCUBES_RENDERCONFIGURATION_H
