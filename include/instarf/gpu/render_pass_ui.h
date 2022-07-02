#ifndef INSTARF_GPU_RENDER_PASS_UI_H
#define INSTARF_GPU_RENDER_PASS_UI_H

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

class RenderPassUi {
public:
  RenderPassUi() = delete;
  explicit RenderPassUi(const Device& device);
  ~RenderPassUi();

  operator VkRenderPass() const noexcept { return renderPass_; }

private:
  VkDevice device_ = VK_NULL_HANDLE;

  VkRenderPass renderPass_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_RENDER_PASS_UI_H
