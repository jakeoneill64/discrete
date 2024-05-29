//
// Created by Jake M O'Neill on 11/09/2022.
//

#include "client.h"

//int client::run()
//{x

//    m_window = discrete::initialiseGLFW();
//    RenderContext renderContext = createGLContext("/Users/jake/Dev/discrete/src/resources/shader/vertex-basic.glsl","/Users/jake/Dev/discrete/src/resources/shader/fragment-basic.glsl");
//
//    while(Client::m_shouldRun){
//
//        if(glfwWindowShouldClose(m_window))
//            m_shouldRun = false;
//
//        glfwPollEvents();
//
//        m_game->update();
//        m_game->draw(m_window, m);
//
//    }

//    return 0;
//}

//Client::Client() noexcept
//:
//m_shouldRun{true}, m_game{new Game{}}
//{
//    logging::configure(discrete::CLIENT_LOGGER_NAME);
//}
//
//
//void Client::stop() {
//    m_shouldRun = false;
//}
//
//Client::~Client() {
//
//    glfwDestroyWindow(m_window);
//
//}
//
//
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


Client &Client::run(const boost::url &serverURL) {
    Client* client{new Client()};
    client->start();
    return *client;
}

void Client::stop() {
    should_run = false;
}

void Client::start() {
    while(should_run){
        
    }
}
