#ifndef INSTARF_GPU_DESCRIPTOR_H
#define INSTARF_GPU_DESCRIPTOR_H

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;
class UniformBufferBase;

class Descriptor {
public:
  Descriptor() = delete;
  Descriptor(const Device& device, VkDescriptorSetLayout layout);
  ~Descriptor();

  operator VkDescriptorSet() const noexcept { return descriptor_; }

  void bind(uint32_t binding, const UniformBufferBase& buffer);

private:
  VkDevice device_ = VK_NULL_HANDLE;

  VkDescriptorPool descriptorPool_ = VK_NULL_HANDLE;
  VkDescriptorSet descriptor_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_DESCRIPTOR_H
