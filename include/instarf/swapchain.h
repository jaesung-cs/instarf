#ifndef INSTARF_SWAPCHAIN_H
#define INSTARF_SWAPCHAIN_H

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

namespace instarf {

class Engine;

class Swapchain {
public:
  Swapchain() = delete;
  Swapchain(const Engine& engine, VkSurfaceKHR surface);
  ~Swapchain();

  auto imageCount() const noexcept { return imageCount_; }

  bool resize(uint32_t width, uint32_t height);

  bool begin();
  auto imageIndex() noexcept { return imageIndex_; }
  auto image() noexcept { return images_[imageIndex_]; }
  auto commandBuffer() { return commandBuffers_[imageIndex_]; }
  void end();

private:
  void createImageViews();
  void destroyImageViews();

  const Engine* engine_ = nullptr;

  VkSurfaceKHR surface_;
  VkSwapchainCreateInfoKHR swapchainInfo_;
  VkSwapchainKHR swapchain_ = nullptr;

  uint32_t imageCount_;
  std::vector<VkImage> images_;
  std::vector<VkImageView> imageViews_;

  VkCommandPool commandPool_;
  std::vector<VkCommandBuffer> commandBuffers_;

  static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
  uint32_t imageIndex_ = 0;
  uint64_t frameIndex_ = 0;
  std::vector<VkSemaphore> imageAcquiredSemaphores_;
  std::vector<VkSemaphore> renderFinishedSemaphores_;
  std::vector<VkFence> renderFinishedFences_;
};

}  // namespace instarf

#endif  // INSTARF_SWAPCHAIN_H
