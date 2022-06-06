#ifndef INSTARF_SWAPCHAIN_H
#define INSTARF_SWAPCHAIN_H

#include <vector>

#include <vulkan/vulkan.h>

namespace instarf {

class Engine;

class Swapchain {
public:
  Swapchain() = delete;
  Swapchain(const Engine& engine, VkSurfaceKHR surface);
  ~Swapchain();

  bool resize(uint32_t width, uint32_t height);

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
};

}  // namespace instarf

#endif  // INSTARF_SWAPCHAIN_H
