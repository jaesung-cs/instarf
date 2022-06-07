#ifndef INSTARF_DESCRIPTOR_H
#define INSTARF_DESCRIPTOR_H

#include <memory>

#include <vulkan/vulkan.h>

#include <instarf/uniform_buffer.h>

namespace instarf {

class Device;

class Descriptor {
public:
  Descriptor() = default;
  Descriptor(Device device, VkDescriptorSetLayout layout);
  ~Descriptor() = default;

  operator VkDescriptorSet() const;

  void bind(uint32_t binding, UniformBufferBase buffer);

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace instarf

#endif  // INSTARF_DESCRIPTOR_H
