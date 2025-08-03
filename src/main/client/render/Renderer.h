#ifndef RENDERER_H
#define RENDERER_H
#include "engine/world.h"


// renderer
// encapsulate instance and device info, as well as a graphics pipeline and manage render pass etc.

// takes an observation of a world view and renders, based on game shaders etc.
// who constructs this? I imagine this is created by a factory method in the engine
// which takes inputs from the game layer

// config layer -> discrete engine renderer factory <- game specific things {shaders}
//                               |
//                            Renderer
class VulkanBase
{
public:
    VulkanBase();
private:

};

class DiscreteRenderer
{
public:
    virtual ~DiscreteRenderer() = default;
    virtual void draw(std::unique_ptr<WorldView> worldView);
};

class VulkanRenderer final : public DiscreteRenderer
{

public:
    void draw(std::unique_ptr<WorldView> worldView) override;
    ~VulkanRenderer() override;
    friend class RenderBase;

private:
    VulkanRenderer();

};

#endif //RENDERER_H
