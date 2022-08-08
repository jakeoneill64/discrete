//
// Created by Jake M O'Neill on 07/08/2022.
//

#ifndef MRONEILLSCUBES_RENDERCLIENT_H
#define MRONEILLSCUBES_RENDERCLIENT_H

#include "RenderConfiguration.h"
#include <memory>

class RenderClient{
public:
    explicit RenderClient(RenderConfiguration config);
    ~RenderClient();

private:
    std::unique_ptr<GLFWwindow> m_window;
    void renderLoop();
};

#endif //MRONEILLSCUBES_RENDERCLIENT_H
