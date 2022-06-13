#include <instarf/gpu/descriptor_layout.h>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

class DescriptorLayout::Impl {
public:
  Impl() = delete;

  Impl(Device device, const DescriptorLayoutInfo& createInfo)
      : device_(device) {
    std::vector<VkDescriptorSetLayoutBinding> bindings(
        createInfo.bindings.size());
    for (int i = 0; i < createInfo.bindings.size(); i++) {
      const auto& bindingInfo = createInfo.bindings[i];

      bindings[i] = {};
      bindings[i].binding = bindingInfo.binding;
      bindings[i].descriptorType = bindingInfo.type;
      bindings[i].descriptorCount = 1;
      bindings[i].stageFlags = bindingInfo.stages;
    }

    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    descriptorLayoutInfo.bindingCount = bindings.size();
    descriptorLayoutInfo.pBindings = bindings.data();
    vkCreateDescriptorSetLayout(device, &descriptorLayoutInfo, nullptr,
                                &layout_);
  }

  ~Impl() { vkDestroyDescriptorSetLayout(device_, layout_, nullptr); }

  operator VkDescriptorSetLayout() const noexcept { return layout_; }

private:
  Device device_;

  VkDescriptorSetLayout layout_;
};

DescriptorLayout::DescriptorLayout(Device device,
                                   const DescriptorLayoutInfo& createInfo)
    : impl_(std::make_shared<Impl>(device, createInfo)) {}

DescriptorLayout::operator VkDescriptorSetLayout() const { return *impl_; }

}  // namespace gpu
}  // namespace instarf
