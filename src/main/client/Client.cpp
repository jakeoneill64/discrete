#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Client.h"
#include "input/input.h"
#include "persistence/database.h"
#include "persistence/sqlite3.h"
#include <string>

Client::Client()
:
    m_shouldRun{true},
    m_eventManager{},
    m_configRepository{m_eventManager, m_database}
{

    auto dbPointer = m_database.get();
    sqlite3_open("discrete.db", &dbPointer);

    sqlite3_update_hook(
        m_database.get(),
        dbEventCallback,
        &m_eventManager
    );

    if(!glfwInit()){
        // TODO handle.
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        m_configRepository[""].transform(
            [](const auto version){
                return std::stoi(version);
            }
        ).value_or(4)
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        m_configRepository[""].transform(
            [](const auto version){
                return std::stoi(version);
            }
        ).value_or(6)
    );

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    m_window = std::unique_ptr<GLFWwindow, DestroyGLFWWindow>(
            glfwCreateWindow(
            m_configRepository[""].transform(
                [](const auto version){
                    return std::stoi(version);
                }
            ).value_or(6),
            m_configRepository[""].transform(
                [](const auto version){
                    return std::stoi(version);
                }
            ).value_or(6),
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
        instance().m_eventManager->publishEvent<KeyEvent>({key, action});
    });

    glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xPos, double yPos){
        instance().m_eventManager->publishEvent<MousePositionEvent>({xPos, yPos});
    });

    glfwSetMouseButtonCallback(m_window.get(), [](GLFWwindow* window, int button, int action, int mods){
        instance().m_eventManager->publishEvent<MouseButtonEvent>({button, action, mods});
    });

    m_eventManager->subscribeEvent<KeyEvent>([](KeyEvent event){
        if(event.action == GLFW_REPEAT || event.action == GLFW_PRESS){
             char character{GLFW_KEY_MAPPINGS.at(event.key)};
        }
    });


}

void DestroyGLFWWindow::operator()(GLFWwindow *ptr) const {

    glfwDestroyWindow(ptr);

}
