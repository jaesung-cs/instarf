#ifndef INSTARF_GPU_DESCRIPTOR_LAYOUT_H
#define INSTARF_GPU_DESCRIPTOR_LAYOUT_H

#include <vector>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

struct DescriptorLayoutInfo {
  struct Binding {
    uint32_t binding;
    VkDescriptorType type;
    VkShaderStageFlags stages;
  };
  std::vector<Binding> bindings;
};

class DescriptorLayout {
public:
  DescriptorLayout() = delete;
  DescriptorLayout(const Device& device, const DescriptorLayoutInfo& createInfo);
  ~DescriptorLayout();

  operator VkDescriptorSetLayout() const noexcept { return layout_; }

private:
  VkDevice device_ = VK_NULL_HANDLE;

  VkDescriptorSetLayout layout_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_DESCRIPTOR_LAYOUT_H
