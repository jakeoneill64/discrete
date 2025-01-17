#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Client.h"
#include "input/input.h"
#include "default-client-config.h"
#include <string>

Client::Client()
:
    m_shouldRun{true}
{

    if(!glfwInit()){
        // TODO handle.
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_clientConfig.glMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_clientConfig.glMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    m_window = std::unique_ptr<GLFWwindow, DestroyGLFWWindow>(
            glfwCreateWindow(
            m_clientConfig.initialWindowWidth,
            m_clientConfig.initialWindowHeight,
            "Discrete Engine",
            nullptr,
            nullptr)
    );

    glfwMakeContextCurrent(m_window.get());
    glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        //TODO handle.
    }

    // --- initialise engine ---

    // https://www.glfw.org/docs/3.3/input_guide.html
    glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int scancode, int action, int mods){
        instance().publishEvent<KeyEvent>({key, action});
    });

    glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xPos, double yPos){
        //TODO how does this tranlate to an action. Not trying to couple this tightly
        // MousePositionEvent -> lookup action -> action(mouseEvent)
        instance().publishEvent<MousePositionEvent>({xPos, yPos});
    });

    glfwSetMouseButtonCallback(m_window.get(), [](GLFWwindow* window, int button, int action, int mods){
        instance().publishEvent<MouseButtonEvent>({button, action, mods});
    });

    subscribeEvent<KeyEvent>([](KeyEvent event){
        if(event.action == GLFW_REPEAT || event.action == GLFW_PRESS){
             char character{GLFW_KEY_MAPPINGS.at(event.key)};
        }
    });


}

void DestroyGLFWWindow::operator()(GLFWwindow *ptr) const {

    glfwDestroyWindow(ptr);

}
