//
// Created by Jake M O'Neill on 07/08/2022.
//

#ifndef MRONEILLSCUBES_RENDER_H
#define MRONEILLSCUBES_RENDER_H

#include "Camera.h"
#include "Mesh.h"
#include "GLFW/glfw3.h"
#include <memory>
#include <vector>
#include <spdlog/spdlog.h>
#include "constants.h"

namespace discrete {

#ifdef DISCRETE_DEBUG
#  define glWrap(x) discrete::glClearError(); x; discrete::glLogError(#x, __FILE__, __LINE__);

    static void glClearError(){
        while(glGetError() != GL_NO_ERROR);
    }

    static void glLogError(const char* functionName, const char* fileName, int line){
        GLenum errorCode;
        while((errorCode = glGetError()) != GL_NO_ERROR){
            spdlog::get(discrete::CLIENT_LOGGER_NAME)->error(
                    std::string{"gl error: "} +
                    std::to_string(errorCode) +
                    "in " + functionName + " (" + fileName + ":" + std::to_string(line) + ")"
            );
        }
    }
#else
#define glWrap(x) x;
#endif

    GLFWwindow* initialiseGLFW();

    struct RenderContext{ // contain all metadata needed to draw
        unsigned int vbo;
        unsigned int vao;
        unsigned int ebo;
        unsigned int programId;
    };

    RenderContext createGLContext(const char* vertexShaderPath, const char* fragmentShaderPath);

    void draw(const Camera& camera, GLFWwindow *window, const std::vector<Mesh>& meshes, const RenderContext& renderContext);

}



#endif //MRONEILLSCUBES_RENDER_H
