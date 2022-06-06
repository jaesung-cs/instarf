#include <instarf/descriptor_layout.h>

#include <instarf/engine.h>

namespace instarf {

class DescriptorLayout::Impl {
public:
  Impl() = delete;

  Impl(Engine engine, const DescriptorLayoutInfo& createInfo)
      : engine_(engine) {
    auto device = engine.device();

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

  ~Impl() {
    auto device = engine_.device();
    vkDestroyDescriptorSetLayout(device, layout_, nullptr);
  }

  operator VkDescriptorSetLayout() const noexcept { return layout_; }

private:
  Engine engine_;

  VkDescriptorSetLayout layout_;
};

DescriptorLayout::DescriptorLayout(Engine engine,
                                   const DescriptorLayoutInfo& createInfo)
    : impl_(std::make_shared<Impl>(engine, createInfo)) {}

DescriptorLayout::operator VkDescriptorSetLayout() const { return *impl_; }

}  // namespace instarf
