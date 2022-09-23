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

int Client::run()
{

    while(Client::s_shouldRun){

        if(glfwWindowShouldClose(m_window))
            s_shouldRun = false;


        update();
        discrete::draw();

    }

    return 0;
}

void Client::update() {

    glfwPollEvents();

}


Client::Client()
{
}

Client::~Client() {

    glfwDestroyWindow(m_window);

}

