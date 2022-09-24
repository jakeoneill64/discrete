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


bool Client::s_shouldRun = true;
Client Client::instance;

int Client::run()
{
    //throwaway stuff
    Camera cam{};
    cam.m_targetPosition = glm::vec3{0, 0, 0};
    cam.m_position = glm::vec3{-1.0f, 0.0f, 0.0f};

    std::vector<Mesh> meshes;
    meshes.push_back(GreenBlock{}.getMesh());

    m_window = discrete::initialiseGLFW();
    discrete::RenderContext renderContext = discrete::createGLContext("/Users/jake/Dev/discrete/src/resources/shader/vertex-basic.glsl","/Users/jake/Dev/discrete/src/resources/shader/fragment-basic.glsl");

    while(Client::s_shouldRun){

        if(glfwWindowShouldClose(m_window))
            s_shouldRun = false;


        update();
        discrete::draw(cam, m_window, meshes, renderContext);

    }

    return 0;
}

Client::Client() noexcept {
    logging::configure(discrete::CLIENT_LOGGER_NAME);
}

void Client::update() {

    glfwPollEvents();

}

Client::~Client() {

    glfwDestroyWindow(m_window);

}

