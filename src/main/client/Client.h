//
// Created by Jake M O'Neill on 11/09/2022.
//

#ifndef MRONEILLSCUBES_CLIENT_H
#define MRONEILLSCUBES_CLIENT_H

#include <memory>
#include <GLFW/glfw3.h>
#include "client/render/render.h"

class Client{

public:

    Client();
    int run();
    ~Client();


private:

    void update();

    static bool s_shouldRun;

    GLFWwindow* m_window;
    std::unique_ptr<discrete::render> renderer;

};

#endif //MRONEILLSCUBES_CLIENT_H
