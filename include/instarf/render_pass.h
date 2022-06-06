#ifndef INSTARF_RENDER_PASS_H
#define INSTARF_RENDER_PASS_H

#include <memory>

#include <vulkan/vulkan.h>

namespace instarf {

class Engine;

class RenderPass {
public:
  RenderPass() = default;
  explicit RenderPass(Engine engine);
  ~RenderPass() = default;

  operator VkRenderPass() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}  // namespace instarf

#endif  // INSTARF_RENDER_PASS_H
