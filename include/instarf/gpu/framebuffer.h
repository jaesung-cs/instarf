#ifndef INSTARF_GPU_FRAMEBUFFER_H
#define INSTARF_GPU_FRAMEBUFFER_H

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
  Framebuffer() = delete;
  Framebuffer(const Device& device, const FramebufferInfo& createInfo);
  ~Framebuffer();

  operator VkFramebuffer() const noexcept { return framebuffer_; }

  void resize(uint32_t width, uint32_t height);

private:
  VkDevice device_ = VK_NULL_HANDLE;

  std::vector<VkFormat> formats_;
  std::vector<VkFramebufferAttachmentImageInfo> imageInfos_;
  VkFramebufferAttachmentsCreateInfo attachmentsInfo_ = {};
  VkFramebufferCreateInfo framebufferInfo_ = {};
  VkFramebuffer framebuffer_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_FRAMEBUFFER_H
