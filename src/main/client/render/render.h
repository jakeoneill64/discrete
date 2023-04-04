//
// Created by Jake M O'Neill on 07/08/2022.
//

#ifndef MRONEILLSCUBES_RENDER_H
#define MRONEILLSCUBES_RENDER_H

#include "mesh.h"
#include "GLFW/glfw3.h"
#include <memory>
#include <vector>
#include <spdlog/spdlog.h>
#include "constants.h"

#ifdef DISCRETE_DEBUG
#  define glWrap(x) glClearError(); x; glLogError(#x, __FILE__, __LINE__);

    static void glClearError(){
        while(glGetError() != GL_NO_ERROR);
    }

    static void glLogError(const char* functionName, const char* fileName, int line){
        GLenum errorCode;
        while((errorCode = glGetError()) != GL_NO_ERROR){
            spdlog::get(discrete::CLIENT_LOGGER_NAME)->error(
                    std::string{"gl error: "} +
                    std::to_string(errorCode) +
                    " in " + functionName + " (" + fileName + ":" + std::to_string(line) + ")"
            );
        }
    }


#else
#define glWrap(x) x;
#endif

struct RenderContext{ // contain all metadata needed to draw
        unsigned int vbo;
        unsigned int vao;
        unsigned int ebo;
        unsigned int programId;
};

RenderContext createGLContext(const char* vertexShaderPath, const char* fragmentShaderPath);



void draw(glm::mat4 view, GLFWwindow *window, const std::vector<mesh>& meshes, const RenderContext& renderContext);

class Renderer{
public:
    virtual void draw(glm::mat4 view, const std::vector<mesh>& meshes);
private:
    std::shared_ptr<GLFWwindow> m_window;
    RenderContext m_renderContext;
};





#endif //MRONEILLSCUBES_RENDER_H