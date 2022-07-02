#include <instarf/gpu/descriptor.h>

#include <instarf/gpu/device.h>

#include <instarf/gpu/uniform_buffer.h>

namespace instarf {
namespace gpu {

Descriptor::Descriptor(const Device& device, VkDescriptorSetLayout layout) : device_(device), descriptorPool_(device.descriptorPool()) {
  VkDescriptorSetAllocateInfo descriptorInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
  descriptorInfo.descriptorPool = descriptorPool_;
  descriptorInfo.descriptorSetCount = 1;
  descriptorInfo.pSetLayouts = &layout;
  vkAllocateDescriptorSets(device, &descriptorInfo, &descriptor_);
}

Descriptor::~Descriptor() { vkFreeDescriptorSets(device_, descriptorPool_, 1, &descriptor_); }

void Descriptor::bind(uint32_t binding, const UniformBufferBase& buffer) {
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

}  // namespace gpu
}  // namespace instarf
