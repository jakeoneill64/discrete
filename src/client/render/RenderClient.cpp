//
// Created by Jake M O'Neill on 07/08/2022.
//

#include "RenderClient.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

Render::RenderClient::RenderClient(RenderConfiguration& config, const Camera* const activeCamera):
m_activeCamera{activeCamera}

{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.glMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.glMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, config.glForwardCompatible);
    #endif

    m_window = std::unique_ptr<GLFWwindow> (glfwCreateWindow(
            config.windowStartWidth,
            config.windowStartHeight,
            config.windowName.c_str(),
            nullptr,
            nullptr));

    glfwMakeContextCurrent(m_window.get());

    glfwSetFramebufferSizeCallback(m_window.get(), &RenderClient::onResize);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("could not load the OpenGL function pointers");
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(m_window.get()))
    {
        // input
        // -----

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_window.get());
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}


void Render::RenderClient::onResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Render::RenderClient::renderLoop() {

}

void Render::RenderClient::setActiveCamera(Camera *camera) {
    m_activeCamera = camera;
}







