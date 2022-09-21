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
#define DISCRETE_DEBUG


namespace discrete {

#ifdef DISCRETE_DEBUG
#  define glWrap(x) discrete::glClearError(); x; discrete::glLogError(#x, __FILE__, __LINE__);

    static void glClearError(){
        while(glGetError() != GL_NO_ERROR);
    }

    static void glLogError(const char* functionName, const char* fileName, int line){
        GLenum errorCode;
        while((errorCode = glGetError()) != GL_NO_ERROR){
            spdlog::get("logger")->error(
                    std::string{"gl error: "} +
                    std::to_string(errorCode) +
                    "in " + functionName + " (" + fileName + ":" + std::to_string(line) + ")"
            );
        }
    }
#else
#define glWrap(x) x;
#endif

    class render {
    public:

        ~render();

        render();

        void addMesh(const Mesh &mesh);

        void draw(Camera camera, GLFWwindow *window);


    private:

        //we may use more than one of these in the future.
        unsigned int VBO;
        unsigned int VAO;
        unsigned int EBO;

        unsigned int shaderProgramId;
        glm::mat4 projection;

        std::vector<const Mesh> m_meshes;
    };

}



#endif //MRONEILLSCUBES_RENDER_H
