#include <instarf/gpu/descriptor_layout.h>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

DescriptorLayout::DescriptorLayout(const Device& device, const DescriptorLayoutInfo& createInfo) : device_(device) {
  std::vector<VkDescriptorSetLayoutBinding> bindings(createInfo.bindings.size());
  for (int i = 0; i < createInfo.bindings.size(); i++) {
    const auto& bindingInfo = createInfo.bindings[i];

    bindings[i] = {};
    bindings[i].binding = bindingInfo.binding;
    bindings[i].descriptorType = bindingInfo.type;
    bindings[i].descriptorCount = 1;
    bindings[i].stageFlags = bindingInfo.stages;
  }

  VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  descriptorLayoutInfo.bindingCount = bindings.size();
  descriptorLayoutInfo.pBindings = bindings.data();
  vkCreateDescriptorSetLayout(device_, &descriptorLayoutInfo, nullptr, &layout_);
}

DescriptorLayout::~DescriptorLayout() { vkDestroyDescriptorSetLayout(device_, layout_, nullptr); }

}  // namespace gpu
}  // namespace instarf
