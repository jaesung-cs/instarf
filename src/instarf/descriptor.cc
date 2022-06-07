#include <instarf/descriptor.h>

#include <instarf/device.h>

namespace instarf {

class Descriptor::Impl {
public:
  Impl() = delete;

  Impl(Device device, VkDescriptorSetLayout layout) : device_(device) {
    auto descriptorPool = device.descriptorPool();

    VkDescriptorSetAllocateInfo descriptorInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    descriptorInfo.descriptorPool = descriptorPool;
    descriptorInfo.descriptorSetCount = 1;
    descriptorInfo.pSetLayouts = &layout;
    vkAllocateDescriptorSets(device, &descriptorInfo, &descriptor_);
  }

  ~Impl() {
    auto descriptorPool = device_.descriptorPool();

    vkFreeDescriptorSets(device_, descriptorPool, 1, &descriptor_);
  }

  operator VkDescriptorSet() const noexcept { return descriptor_; }

  void bind(uint32_t binding, UniformBufferBase buffer) {
    VkDescriptorBufferInfo bufferInfo = {buffer, 0, buffer.alignment()};

    VkWriteDescriptorSet write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    write.dstSet = descriptor_;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device_, 1, &write, 0, nullptr);
  }

private:
  Device device_;

  VkDescriptorSet descriptor_;
};

Descriptor::Descriptor(Device device, VkDescriptorSetLayout layout)
    : impl_(std::make_shared<Impl>(device, layout)) {}

Descriptor::operator VkDescriptorSet() const { return *impl_; }

void Descriptor::bind(uint32_t binding, UniformBufferBase buffer) {
  impl_->bind(binding, buffer);
}

}  // namespace instarf
