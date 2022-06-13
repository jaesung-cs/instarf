#ifndef INSTARF_GPU_FRAMEBUFFER_H
#define INSTARF_GPU_FRAMEBUFFER_H

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

struct FramebufferInfo {
  VkRenderPass renderPass;

  struct ImageInfo {
    VkImageUsageFlags usage;
    VkFormat format;
  };
  std::vector<ImageInfo> imageInfos;
};

class Framebuffer {
public:
  Framebuffer() = default;
  Framebuffer(Device device, const FramebufferInfo& createInfo);
  ~Framebuffer() = default;

  operator VkFramebuffer() const;

  void resize(uint32_t width, uint32_t height);

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_FRAMEBUFFER_H
