#ifndef INSTARF_GPU_RENDER_PASS_H
#define INSTARF_GPU_RENDER_PASS_H

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

class RenderPass {
public:
  RenderPass() = delete;
  explicit RenderPass(const Device& device);
  ~RenderPass();

  operator VkRenderPass() const noexcept { return renderPass_; }

private:
  VkDevice device_ = VK_NULL_HANDLE;

  VkRenderPass renderPass_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_RENDER_PASS_H
