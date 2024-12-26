#include "client.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <memory>
#include "input/input.h"
#include "yaml-cpp/yaml.h"
#include "default-client-config.dat"


Client::Client()
:
    m_shouldRun{true},
    m_clientConfig{}
{

    YAML::Node node = YAML::Load(default_client_config);

    if(!glfwInit()){
        // TODO handle.
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_clientConfig.GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_clientConfig.GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    m_window = std::unique_ptr<GLFWwindow, DestroyGLFWWindow>(
            glfwCreateWindow(
            m_clientConfig.WINDOW_START_WIDTH,
            m_clientConfig.WINDOW_START_HEIGHT,
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

    /*! https://www.glfw.org/docs/3.3/input_guide.html
     *  If you wish to know what the state of the Caps Lock and Num Lock keys was when input events were generated,
     *  set the GLFW_LOCK_KEY_MODS input mode. glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
     */
    glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int scancode, int action, int mods){
        instance().m_engine.publishEvent<KeyEvent>({key, scancode, action, mods});
    });

    glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xPos, double yPos){
        instance().m_engine.publishEvent<MouseEvent>({xPos, yPos});
    });

    // initialise engine

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
