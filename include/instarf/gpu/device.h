#ifndef INSTARF_GPU_DEVICE_H
#define INSTARF_GPU_DEVICE_H

#include <vector>
#include <string>
#include <functional>

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace instarf {
namespace gpu {

struct DeviceInfo {
  std::vector<std::string> extensions;
};

class Device {
protected:
  using CommandRecordFunc = std::function<void(VkCommandBuffer)>;

public:
  Device() = delete;
  Device(VkInstance instance, const DeviceInfo& createInfo);
  ~Device();

  operator VkDevice() const noexcept { return device_; }

  auto instance() const noexcept { return instance_; }
  auto physicalDevice() const noexcept { return physicalDevice_; }
  int queueIndex() const noexcept { return queueIndex_; }
  auto queue() const noexcept { return queue_; }
  auto allocator() const noexcept { return allocator_; }
  auto descriptorPool() const noexcept { return descriptorPool_; }

  void submit(CommandRecordFunc command) const;
  void waitIdle();

private:
  VkInstance instance_ = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  int queueIndex_ = -1;
  VkQueue queue_ = VK_NULL_HANDLE;

  VmaAllocator allocator_ = VK_NULL_HANDLE;
  VkDescriptorPool descriptorPool_ = VK_NULL_HANDLE;
  VkCommandPool commandPool_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_DEVICE_H
