#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "Client.h"
#include "input/input.h"
#include "persistence/database.h"
#include "persistence/sqlite3.h"
#include "engine/action/EntityLook.h"
#include "render/render.h"
#include <string>

Client::Client()
:
    m_shouldRun{true},
    m_eventManager{},
    m_database(),
    m_configRepository{m_eventManager, m_database},
    m_inputRepository(m_eventManager, m_database)
{

    auto dbPointer = m_database.get();
    sqlite3_open("discrete.db", &dbPointer);

    sqlite3_update_hook(
        m_database.get(),
        dbEventCallback,
        &m_eventManager
    );

    if(!glfwInit()){
        throw std::runtime_error("GLFW failed to initialise");
    }

    // Ensure GLFW does NOT create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = std::unique_ptr<GLFWwindow, DestroyGLFWWindow>(
            glfwCreateWindow(
            m_configRepository["client.window.initial_width"].transform(
                [](const auto version){
                    return std::stoi(version);
                }
            ).value_or(6),
            m_configRepository["client.window.initial_height"].transform(
                [](const auto version){
                    return std::stoi(version);
                }
            ).value_or(6),
            m_configRepository["client.window.name"].value_or("Discrete Engine").c_str(),
            nullptr,
            nullptr)
    );

    glfwMakeContextCurrent(m_window.get());
    glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    uint32_t glfwExtensionCount{0};
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> allInstanceExtensions{glfwExtensions, glfwExtensions + glfwExtensionCount};
    allInstanceExtensions.insert(allInstanceExtensions.end(), INSTANCE_EXTENSION.begin(), INSTANCE_EXTENSION.end());

    VkInstanceCreateInfo instanceCreateInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType sType;
        nullptr,                                // const void* pNext;
        0,                                      // VkInstanceCreateFlags flags;
        nullptr,                                // const VkApplicationInfo* pApplicationInfo;
#ifdef DISCRETE_DEBUG
        static_cast<uint32_t>(VALIDATION_LAYERS.size()),
        VALIDATION_LAYERS.data(),
#else
        0,                                      // uint32_t enabledLayerNameCount;
        nullptr,                                // const char* const* ppEnabledLayerNames;
#endif
        2,                                      // uint32_t enabledExtensionNameCount;
        allInstanceExtensions.data(),           // const char* const* ppEnabledExtensionNames;
    };

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_vulkanInstance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    VkSurfaceKHR surface; // perhaps a member of client. I don't really want a bunch of low-level vulkan stuff in client.
    if (glfwCreateWindowSurface(m_vulkanInstance, m_window.get(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan surface!");
    }

    // --- initialise engine ---

    // https://www.glfw.org/docs/3.3/input_guide.html
    glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int scancode, int action, int mods){
        instance().m_eventManager->publishEvent<KeyEvent>({key, action});
    });

    glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xPos, double yPos){
        instance().m_eventManager->publishEvent<MousePositionEvent>({xPos, yPos});
    });

    glfwSetMouseButtonCallback(m_window.get(), [](GLFWwindow* window, int button, int action, int mods){
        instance().m_eventManager->publishEvent<MouseButtonEvent>({button, action, mods});
    });

    m_eventManager->subscribeEvent<KeyEvent>(
        [&](KeyEvent event){
        if(event.action != GLFW_REPEAT && event.action != GLFW_PRESS)
            return;
        m_engine.submit<void>(
            m_inputRepository[GLFW_KEY_MAPPINGS.at(event.key),
            m_engine.getInputContext(m_boundEntityId)]
        );
    });

    m_eventManager->subscribeEvent<MouseButtonEvent>(
        [&](KeyEvent event) {
            if (event.action != GLFW_REPEAT && event.action != GLFW_PRESS)
                return;
            m_engine.submit<void>(
                    m_inputRepository[GLFW_MOUSE_BUTTON_MAPPINGS.at(event.key),
                            m_engine.getInputContext(m_boundEntityId)]
            );
        }
    );

    m_eventManager->subscribeEvent<MousePositionEvent>(
        [&](MousePositionEvent event) {
        EntityLook{{m_boundEntityId, event.xPos, event.yPos}};
//            m_engine.submit<EntityLookParam>(
//                    std::make_unique<EntityLook>(EntityLook{{m_boundEntityId, event.xPos, event.yPos}})
//            );
        }
    );

}

void Client::loop() {
    while(m_shouldRun){
        m_eventManager->publishEvent(RenderStartEvent{});
    }
}

void DestroyGLFWWindow::operator()(GLFWwindow *ptr) const {

    glfwDestroyWindow(ptr);

}
