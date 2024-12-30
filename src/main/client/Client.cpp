#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Client.h"
#include "input/input.h"
#include "default-client-config.h"
#include <string>

Client::Client()
:
    m_shouldRun{true},
    m_clientConfig{}
{

//    YAML::Node node = YAML::Load(std::string{reinterpret_cast<char*>(default_client_config), default_client_config_len});
//    auto x = node["graphics"].as<ClientConfig>();

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
        instance().m_engine.publishEvent<KeyEvent>({key, scancode, action, mods});
    });

    glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xPos, double yPos){
        instance().m_engine.publishEvent<MousePositionEvent>({xPos, yPos});
    });

    glfwSetMouseButtonCallback(m_window.get(), [](GLFWwindow* window, int button, int action, int mods){
        instance().m_engine.publishEvent<MouseButtonEvent>({button, action, mods});
    });

    m_engine.subscribeEvent<KeyEvent>([this](KeyEvent event){


        if(event.action == GLFW_REPEAT || event.action == GLFW_PRESS){
            try {
                char character = GLFW_KEY_MAPPINGS.at(event.key);

            }catch(const std::logic_error& _) {

            }
        }
    });


}

void DestroyGLFWWindow::operator()(GLFWwindow *ptr) const {

    glfwDestroyWindow(ptr);

}
