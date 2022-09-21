//
// Created by Jake M O'Neill on 11/09/2022.
//


#include "glad/glad.h"
#include <glm/glm.hpp>
#include "Client.h"
#include "Log.h"
#include "client/Constants.h"
#include "Voxel.h"
#include "client/render/GreenBlock.h"


bool Client::s_shouldRun = true;

int Client::run()
{

    m_window = glfwCreateWindow(
            discrete::WINDOW_START_WIDTH,
            discrete::WINDOW_START_HEIGHT,
            discrete::ENGINE_NAME,
            nullptr,
            nullptr
    );


    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow *window, int width, int height){glViewport(0, 0, width, height);});

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    renderer = std::make_unique<discrete::render>(discrete::render());

    Camera cam{};
    cam.m_targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    cam.m_position = glm::vec3(0.0f, -5.0f, 0.0);

    GreenBlock gb{};

    Voxel* vox = &gb;

    renderer->addMesh(vox->getMesh());


    while(Client::s_shouldRun){

        if(glfwWindowShouldClose(m_window))
            s_shouldRun = false;


        update();
        renderer->draw(cam, m_window);

    }

    return 0;
}

void Client::update() {

    glfwPollEvents();

}


Client::Client()
{
    logging::configure("client");

    if(!glfwInit()){
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, discrete::GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, discrete::GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);




#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
}

Client::~Client() {

    glfwDestroyWindow(m_window);

}

