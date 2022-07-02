#include <instarf/gpu/swapchain.h>

#include <iostream>
#include <vector>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

class Swapchain::Impl {
public:
  Impl(Device device, VkSurfaceKHR surface)
      : device_(device), surface_(surface) {
    imageCount_ = 3;

    swapchainInfo_ = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchainInfo_.surface = surface_;
    swapchainInfo_.minImageCount = imageCount_;
    swapchainInfo_.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapchainInfo_.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo_.imageExtent = {0, 0};
    swapchainInfo_.imageArrayLayers = 1;
    swapchainInfo_.imageUsage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapchainInfo_.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainInfo_.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo_.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchainInfo_.clipped = true;

    // Command pool
    VkCommandPoolCreateInfo commandPoolInfo = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = device.queueIndex();
    vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool_);

    VkCommandBufferAllocateInfo commandBufferInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    commandBufferInfo.commandPool = commandPool_;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = imageCount_;
    commandBuffers_.resize(imageCount_);
    vkAllocateCommandBuffers(device, &commandBufferInfo,
                             commandBuffers_.data());

    // Synchronization
    VkSemaphoreCreateInfo semaphoreInfo = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    renderFinishedSemaphores_.resize(imageCount_);
    renderFinishedFences_.resize(imageCount_);
    for (uint32_t i = 0; i < imageCount_; i++) {
      vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &renderFinishedSemaphores_[i]);
      vkCreateFence(device, &fenceInfo, nullptr, &renderFinishedFences_[i]);
    }

    imageAcquiredSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &imageAcquiredSemaphores_[i]);
    }

    frameFinishedFences_ =
        std::vector<VkFence>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
  }

  ~Impl() {
    std::vector<VkFence> renderingFences;
    for (auto fence : frameFinishedFences_) {
      if (fence) renderingFences.push_back(fence);
    }

    if (!renderingFences.empty()) {
      vkWaitForFences(device_, static_cast<uint32_t>(renderingFences.size()),
                      renderingFences.data(), VK_TRUE, UINT64_MAX);
    }

    for (auto semaphore : imageAcquiredSemaphores_)
      vkDestroySemaphore(device_, semaphore, nullptr);

    for (auto semaphore : renderFinishedSemaphores_)
      vkDestroySemaphore(device_, semaphore, nullptr);

    for (auto fence : renderFinishedFences_)
      vkDestroyFence(device_, fence, nullptr);

    vkDestroyCommandPool(device_, commandPool_, nullptr);

    destroyImageViews();
    vkDestroySwapchainKHR(device_, swapchain_, nullptr);

    auto instance = device_.instance();
    vkDestroySurfaceKHR(instance, surface_, nullptr);
  }

  auto imageCount() const noexcept { return imageCount_; }
  auto imageUsage() const noexcept { return swapchainInfo_.imageUsage; }
  auto format() const noexcept { return swapchainInfo_.imageFormat; }

  bool resize(uint32_t width, uint32_t height) {
    if (swapchainInfo_.imageExtent.width != width ||
        swapchainInfo_.imageExtent.height != height) {
      std::vector<VkFence> renderingFences;
      for (auto fence : frameFinishedFences_) {
        if (fence) renderingFences.push_back(fence);
      }

      if (!renderingFences.empty()) {
        vkWaitForFences(device_, static_cast<uint32_t>(renderingFences.size()),
                        renderingFences.data(), VK_TRUE, UINT64_MAX);
        vkResetFences(device_, static_cast<uint32_t>(renderingFences.size()),
                      renderingFences.data());
      }

      for (auto& fence : frameFinishedFences_) fence = VK_NULL_HANDLE;

      destroyImageViews();

      swapchainInfo_.imageExtent = {width, height};
      swapchainInfo_.oldSwapchain = swapchain_;

      VkSwapchainKHR newSwapchain;
      vkCreateSwapchainKHR(device_, &swapchainInfo_, nullptr, &newSwapchain);
      if (swapchain_) vkDestroySwapchainKHR(device_, swapchain_, nullptr);
      swapchain_ = newSwapchain;

      createImageViews();

      return true;
    }

    return false;
  }

  bool begin() {
    auto frameIndex = frameIndex_ % MAX_FRAMES_IN_FLIGHT;

    auto result = vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX,
                                        imageAcquiredSemaphores_[frameIndex],
                                        nullptr, &imageIndex_);

    switch (result) {
        // Success
      case VK_SUBOPTIMAL_KHR:
        std::cout << "Subobtimal next image" << std::endl;
        // Fall through
      case VK_SUCCESS:
        return true;

        // Failure
      default:
        return false;
    }
  }

  auto imageIndex() noexcept { return imageIndex_; }
  auto image() { return images_[imageIndex_]; }
  auto imageView() { return imageViews_[imageIndex_]; }
  auto commandBuffer() { return commandBuffers_[imageIndex_]; }

  void end() {
    auto queue = device_.queue();
    auto frameIndex = frameIndex_ % MAX_FRAMES_IN_FLIGHT;

    if (frameFinishedFences_[frameIndex]) {
      vkWaitForFences(device_, 1, &frameFinishedFences_[frameIndex], VK_TRUE,
                      UINT64_MAX);
      vkResetFences(device_, 1, &frameFinishedFences_[frameIndex]);
      frameFinishedFences_[frameIndex] = VK_NULL_HANDLE;
    }

    std::vector<VkPipelineStageFlags> waitStages = {
        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };
    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAcquiredSemaphores_[frameIndex];
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers_[imageIndex_];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores_[imageIndex_];
    vkQueueSubmit(queue, 1, &submitInfo, renderFinishedFences_[imageIndex_]);

    frameFinishedFences_[frameIndex] = renderFinishedFences_[imageIndex_];

    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores_[imageIndex_];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain_;
    presentInfo.pImageIndices = &imageIndex_;
    auto result = vkQueuePresentKHR(queue, &presentInfo);

    switch (result) {
      case VK_SUBOPTIMAL_KHR:
        std::cout << "Subobtimal present" << std::endl;
    }

    frameIndex_++;
  }

private:
  void createImageViews() {
    vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount_, nullptr);
    images_.resize(imageCount_);
    vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount_, images_.data());

    VkImageViewCreateInfo imageViewInfo = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = swapchainInfo_.imageFormat;
    imageViewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    imageViews_.resize(imageCount_);
    for (int i = 0; i < images_.size(); i++) {
      imageViewInfo.image = images_[i];
      vkCreateImageView(device_, &imageViewInfo, nullptr, &imageViews_[i]);
    }
  }

  void destroyImageViews() {
    for (auto imageView : imageViews_)
      vkDestroyImageView(device_, imageView, nullptr);
    images_.clear();
    imageViews_.clear();
  }

  Device device_;

  VkSurfaceKHR surface_;
  VkSwapchainCreateInfoKHR swapchainInfo_;
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;

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
  std::vector<VkFence> frameFinishedFences_;
};

Swapchain::Swapchain(Device device, VkSurfaceKHR surface)
    : impl_(std::make_shared<Impl>(device, surface)) {}

uint32_t Swapchain::imageCount() const { return impl_->imageCount(); }
VkImageUsageFlags Swapchain::imageUsage() const { return impl_->imageUsage(); }
VkFormat Swapchain::format() const { return impl_->format(); }

bool Swapchain::resize(uint32_t width, uint32_t height) {
  return impl_->resize(width, height);
}

bool Swapchain::begin() { return impl_->begin(); }
uint32_t Swapchain::imageIndex() { return impl_->imageIndex(); }
VkImage Swapchain::image() { return impl_->image(); }
VkImageView Swapchain::imageView() { return impl_->imageView(); }
VkCommandBuffer Swapchain::commandBuffer() { return impl_->commandBuffer(); }
void Swapchain::end() { impl_->end(); }

}  // namespace gpu
}  // namespace instarf
