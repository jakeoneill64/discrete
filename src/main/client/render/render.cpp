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

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)discrete::WINDOW_START_WIDTH / (float)discrete::WINDOW_START_HEIGHT, 0.1f, 100.0f);

    glWrap(glUniformMatrix4fv(glGetUniformLocation(renderContext.programId, "view"), 1, GL_FALSE, &camera.getViewMatrix()[0][0]));
    glWrap(glUniformMatrix4fv(glGetUniformLocation(renderContext.programId, "projection"), 1, GL_FALSE, &projection[0][0]));

    glWrap(void *vertexBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    glWrap(void *indexBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

    unsigned long indexPointerOffset{0};
    unsigned long vertexPointerOffset{0};

    for(const Mesh& mesh: meshes){
        memcpy(vertexBuffer, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));
        memcpy(indexBuffer, mesh.indices.data(), mesh.indices.size() * sizeof(unsigned int));
        vertexPointerOffset += sizeof(Vertex) * mesh.vertices.size();
        indexPointerOffset += sizeof(unsigned int) * mesh.indices.size();
    }

    glWrap(glUnmapBuffer(GL_ARRAY_BUFFER));
    glWrap(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));

    glWrap(glDrawElements(GL_TRIANGLES, indexPointerOffset / sizeof(unsigned int), GL_UNSIGNED_INT, 0));

    glfwSwapBuffers(window);


}

discrete::RenderContext discrete::createGLContext(const char* vertexShaderPath, const char* fragmentShaderPath) {

    Shader vertex{vertexShaderPath, GL_VERTEX_SHADER};
    Shader fragment{fragmentShaderPath, GL_FRAGMENT_SHADER};
    unsigned int programId{Shader::linkShaders(vertex, fragment)};
    glWrap(glUseProgram(programId));

    unsigned int vao, vbo, ebo;

    glWrap(glGenVertexArrays(1, &vao));
    glWrap(glGenBuffers(1, &vbo));
    glWrap(glGenBuffers(1, &ebo));

    glWrap(glBindVertexArray(vao));
    glWrap(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    glWrap(glBufferData(GL_ARRAY_BUFFER, discrete::GL_INDEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, discrete::GL_ELEMENT_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

    // position attribute
    glWrap(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0));
    glWrap(glEnableVertexAttribArray(0));

    RenderContext context;

    context.vbo = vbo;
    context.vao = vao;
    context.ebo = ebo;
    context.programId = programId;
    return context;
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


