//
// Created by Jake M O'Neill on 11/09/2022.
//

#ifndef MRONEILLSCUBES_CLIENT_H
#define MRONEILLSCUBES_CLIENT_H

#include <memory>
#include <GLFW/glfw3.h>
#include "client/render/render.h"
#include "client/input/input.h"

namespace discrete{

class Client {

public:

    int run();

    void stop();

    ~Client();


    static Client instance;


private:

    void update();

    Client() noexcept;

    bool m_shouldRun;
    GLFWwindow *m_window;

};

GLFWwindow *initialiseGLFW();

}


#endif //MRONEILLSCUBES_CLIENT_H
