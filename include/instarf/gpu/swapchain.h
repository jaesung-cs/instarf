#ifndef INSTARF_GPU_SWAPCHAIN_H
#define INSTARF_GPU_SWAPCHAIN_H

#include <memory>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Instance;
class Device;

class Swapchain {
public:
  Swapchain() = default;
  Swapchain(Instance instance, Device device, VkSurfaceKHR surface);
  ~Swapchain() = default;

  uint32_t imageCount() const;
  VkImageUsageFlags imageUsage() const;
  VkFormat format() const;

  bool resize(uint32_t width, uint32_t height);

  bool begin();
  uint32_t imageIndex();
  VkImage image();
  VkImageView imageView();
  VkCommandBuffer commandBuffer();
  void end();

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_SWAPCHAIN_H
