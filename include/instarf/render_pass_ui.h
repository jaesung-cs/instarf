#ifndef INSTARF_RENDER_PASS_UI_H
#define INSTARF_RENDER_PASS_UI_H

#include <memory>

#include <vulkan/vulkan.h>

namespace instarf {

class Device;

class RenderPassUi {
public:
  RenderPassUi() = default;
  explicit RenderPassUi(Device device);
  ~RenderPassUi() = default;

  operator VkRenderPass() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace instarf

#endif  // INSTARF_RENDER_PASS_UI_H
