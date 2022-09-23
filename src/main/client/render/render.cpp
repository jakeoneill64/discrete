//
// Created by Jake M O'Neill on 07/08/2022.
//

#include "glad/glad.h"
#include "constants.h"
#include "render.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

void discrete::draw(
        const Camera& camera,
        GLFWwindow* window,
        const std::vector<Mesh>& meshes,
        const discrete::RenderContext& renderContext
        ) {

    glWrap(glUniformMatrix4fv(glGetUniformLocation(renderContext.programId, "view"), 1, GL_FALSE, &camera.getViewMatrix()[0][0]));
    glWrap(glUniformMatrix4fv(glGetUniformLocation(renderContext.programId, "projection"), 1, GL_FALSE, &projection[0][0]));


}

discrete::RenderContext discrete::createGLContext(const char* vertexShaderPath, const char* fragmentShaderPath) {

    Shader vertex{vertexShaderPath, GL_VERTEX_SHADER};
    Shader fragment{fragmentShaderPath, GL_FRAGMENT_SHADER};
    unsigned int programId{Shader::linkShaders(vertex, fragment)};
    unsigned int vao, vbo, ibo;

    glWrap(glGenVertexArrays(1, &vao));
    glWrap(glGenBuffers(1, &vbo));
    glWrap(glGenBuffers(1, &ibo));

    glWrap(glBindVertexArray(vao));
    glWrap(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    glWrap(glBufferData(GL_ARRAY_BUFFER, discrete::GL_BUF_SIZE, nullptr, GL_DYNAMIC_DRAW));

    // position attribute
    glWrap(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    glWrap(glEnableVertexAttribArray(0));

}

GLFWwindow* discrete::initialiseGLFW() {

    auto logger{spdlog::get(discrete::CLIENT_LOGGER_NAME)};

    if(!glfwInit()){
        const char* message = "unable to initialise GLFW";
        logger->error(message);
        throw std::runtime_error(message);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, discrete::GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, discrete::GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow * window = glfwCreateWindow(
            discrete::WINDOW_START_WIDTH,
            discrete::WINDOW_START_HEIGHT,
            discrete::ENGINE_NAME,
            nullptr,
            nullptr
    );

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        const char* message = "unable to load GL function pointers";
        logger->error(message);
        throw std::runtime_error(message);
    }

    return window;
}


