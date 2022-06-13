#ifndef INSTARF_GPU_RENDER_PASS_H
#define INSTARF_GPU_RENDER_PASS_H

#include <memory>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

class RenderPass {
public:
  RenderPass() = default;
  explicit RenderPass(Device device);
  ~RenderPass() = default;

  operator VkRenderPass() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_RENDER_PASS_H
