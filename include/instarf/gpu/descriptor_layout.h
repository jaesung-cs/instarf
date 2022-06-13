#ifndef INSTARF_GPU_DESCRIPTOR_LAYOUT_H
#define INSTARF_GPU_DESCRIPTOR_LAYOUT_H

#include <memory>
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
  DescriptorLayout() = default;
  DescriptorLayout(Device device, const DescriptorLayoutInfo& createInfo);
  ~DescriptorLayout() = default;

  operator VkDescriptorSetLayout() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_DESCRIPTOR_LAYOUT_H
