#ifndef INSTARF_GPU_DEVICE_H
#define INSTARF_GPU_DEVICE_H

#include <memory>
#include <vector>
#include <string>
#include <functional>

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

namespace instarf {
namespace gpu {

class Instance;

struct DeviceInfo {
  std::vector<std::string> extensions;
};

class Device {
protected:
  using CommandRecordFunc = std::function<void(VkCommandBuffer)>;

public:
  Device() = default;
  Device(Instance instance, const DeviceInfo& createInfo);
  ~Device() = default;

  operator VkDevice() const;

  VkPhysicalDevice physicalDevice() const;
  int queueIndex() const;
  VkQueue queue() const;
  VmaAllocator allocator() const;
  VkDescriptorPool descriptorPool() const;

  void submit(CommandRecordFunc command);
  void waitIdle();

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_DEVICE_H
