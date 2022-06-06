#ifndef INSTARF_RENDER_PASS_H
#define INSTARF_RENDER_PASS_H

#include <vulkan/vulkan.h>

namespace instarf {

class Engine;

class RenderPass {
public:
  RenderPass() = delete;
  explicit RenderPass(const Engine& engine);
  ~RenderPass();

  operator VkRenderPass() const noexcept { return renderPass_; }

private:
  const Engine* engine_;

  VkRenderPass renderPass_;
};
}  // namespace instarf

#endif  // INSTARF_RENDER_PASS_H
