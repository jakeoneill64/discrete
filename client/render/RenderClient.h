//
// Created by Jake M O'Neill on 07/08/2022.
//

#ifndef MRONEILLSCUBES_RENDERCLIENT_H
#define MRONEILLSCUBES_RENDERCLIENT_H

#include "config/GLConfig.h"

class RenderClient{
public:
    explicit RenderClient(GLConfig config);
    ~RenderClient();

private:
    void renderLoop();
};

#endif //MRONEILLSCUBES_RENDERCLIENT_H
