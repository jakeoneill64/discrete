//
// Created by Jake M O'Neill on 11/09/2022.
//


#include "glad/glad.h"
#include <glm/glm.hpp>
#include "Client.h"
#include "Log.h"
#include "constants.h"
#include "Voxel.h"
#include "client/render/GreenBlock.h"
#include "client/input/input.h"

using namespace discrete;


Client Client::instance;

int Client::run()
{
    //throwaway stuff
    Camera cam{};
    cam.m_targetPosition = glm::vec3{0, 0, 0};
    cam.m_position = glm::vec3{-5.0f, 0.0f, 4.0f};

    std::vector<Mesh> meshes;
    auto left = GreenBlock{glm::vec3 {0.0f, 0.0f, 0.0f}}.getMesh();

    meshes.push_back(left);

    m_window = discrete::initialiseGLFW();
    discrete::RenderContext renderContext = discrete::createGLContext("/Users/jake/Dev/discrete/src/resources/shader/vertex-basic.glsl","/Users/jake/Dev/discrete/src/resources/shader/fragment-basic.glsl");


    while(Client::m_shouldRun){

        if(glfwWindowShouldClose(m_window))
            m_shouldRun = false;


        update();
        discrete::draw(cam, m_window, meshes, renderContext);

    }

    return 0;
}

Client::Client() noexcept
:
m_shouldRun{true}
{
    logging::configure(discrete::CLIENT_LOGGER_NAME);
}

void Client::update() {

    glfwPollEvents();

}


void Client::stop() {
    m_shouldRun = false;
}

Client::~Client() {

    glfwDestroyWindow(m_window);

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

    discrete::updateInputManager(window, new EntityInputManager{});
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
        discrete::InputManager::activeInputManager->onKeyEvent(window, key, scancode, action, mods);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos){
        discrete::InputManager::activeInputManager->onMouseEvent(window, xpos, ypos);
    });

    return window;
}


