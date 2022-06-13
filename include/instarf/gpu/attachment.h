#ifndef INSTARF_GPU_ATTACHMENT_H
#define INSTARF_GPU_ATTACHMENT_H

#include <memory>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

class Attachment {
public:
  Attachment() = default;
  explicit Attachment(Device device, VkFormat format,
                      VkSampleCountFlagBits samples);
  ~Attachment() = default;

  operator VkImageView() const;

  VkImageUsageFlags usage() const;
  VkFormat format() const;

  void resize(uint32_t width, uint32_t height);

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_ATTACHMENT_H
