#ifndef INSTARF_GPU_SWAPCHAIN_H
#define INSTARF_GPU_SWAPCHAIN_H

#include <vector>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

class Swapchain {
public:
  Swapchain() = delete;
  Swapchain(const Device& device, VkSurfaceKHR surface);
  ~Swapchain();

  auto imageCount() const noexcept { return imageCount_; }
  auto imageUsage() const noexcept { return swapchainInfo_.imageUsage; }
  auto format() const noexcept { return swapchainInfo_.imageFormat; }

  bool resize(uint32_t width, uint32_t height);

  bool begin();
  auto imageIndex() noexcept { return imageIndex_; }
  auto image() { return images_[imageIndex_]; }
  auto imageView() { return imageViews_[imageIndex_]; }
  auto commandBuffer() { return commandBuffers_[imageIndex_]; }
  void end();

private:
  void createImageViews();
  void destroyImageViews();

  VkInstance instance_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  VkQueue queue_ = VK_NULL_HANDLE;

  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkSwapchainCreateInfoKHR swapchainInfo_ = {};
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;

  uint32_t imageCount_ = 0;
  std::vector<VkImage> images_;
  std::vector<VkImageView> imageViews_;

  VkCommandPool commandPool_ = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> commandBuffers_;

  static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
  uint32_t imageIndex_ = 0;
  uint64_t frameIndex_ = 0;
  std::vector<VkSemaphore> imageAcquiredSemaphores_;
  std::vector<VkSemaphore> renderFinishedSemaphores_;
  std::vector<VkFence> renderFinishedFences_;
  std::vector<VkFence> frameFinishedFences_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_SWAPCHAIN_H
