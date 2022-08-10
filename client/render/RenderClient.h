//
// Created by Jake M O'Neill on 07/08/2022.
//

#ifndef MRONEILLSCUBES_RENDERCLIENT_H
#define MRONEILLSCUBES_RENDERCLIENT_H

#include "RenderConfiguration.h"
#include <memory>

namespace Render {

    class RenderClient {
    public:
        explicit RenderClient(RenderConfiguration& config);
        RenderClient(const RenderClient&) = delete;
        RenderClient& operator=(RenderClient&) = delete;
        ~RenderClient() = default;

    private:
        std::unique_ptr<GLFWwindow> m_window;

        static void onResize(GLFWwindow* window, int width, int height);
        void renderLoop();
    };

}

#endif //MRONEILLSCUBES_RENDERCLIENT_H
