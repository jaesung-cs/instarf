#ifndef INSTARF_GPU_ATTACHMENT_H
#define INSTARF_GPU_ATTACHMENT_H

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace instarf {
namespace gpu {

class Device;

class Attachment {
public:
  Attachment() = delete;
  explicit Attachment(const Device& device, VkFormat format, VkSampleCountFlagBits samples);
  ~Attachment();

  operator VkImageView() const noexcept { return imageView_; }

  auto usage() const noexcept { return imageInfo_.usage; }
  auto format() const noexcept { return imageInfo_.format; }

  void resize(uint32_t width, uint32_t height);

private:
  VkDevice device_ = VK_NULL_HANDLE;
  VmaAllocator allocator_ = VK_NULL_HANDLE;

  VkImageCreateInfo imageInfo_ = {};
  VkImage image_ = VK_NULL_HANDLE;
  VmaAllocationCreateInfo allocationInfo_ = {};
  VmaAllocation allocation_ = VK_NULL_HANDLE;
  VkImageViewCreateInfo imageViewInfo_ = {};
  VkImageView imageView_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_ATTACHMENT_H
