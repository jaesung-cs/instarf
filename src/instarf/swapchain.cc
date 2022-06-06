#include <instarf/swapchain.h>

#include <iostream>

#include <instarf/engine.h>

namespace instarf {

Swapchain::Swapchain(const Engine& engine, VkSurfaceKHR surface)
    : engine_(&engine), surface_(surface) {
  auto device = engine_->device();

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
  commandPoolInfo.queueFamilyIndex = engine_->queueIndex();
  vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool_);

  VkCommandBufferAllocateInfo commandBufferInfo = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  commandBufferInfo.commandPool = commandPool_;
  commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferInfo.commandBufferCount = imageCount_;
  commandBuffers_.resize(imageCount_);
  vkAllocateCommandBuffers(device, &commandBufferInfo, commandBuffers_.data());

  // Synchronization
  VkSemaphoreCreateInfo semaphoreInfo = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  imageAcquiredSemaphores_.resize(imageCount_);
  for (uint32_t i = 0; i < imageCount_; i++)
    vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                      &imageAcquiredSemaphores_[i]);

  VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  fenceInfo.flags = {VK_FENCE_CREATE_SIGNALED_BIT};
  renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedFences_.resize(MAX_FRAMES_IN_FLIGHT);
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                      &renderFinishedSemaphores_[i]);
    vkCreateFence(device, &fenceInfo, nullptr, &renderFinishedFences_[i]);
  }
}

Swapchain::~Swapchain() {
  auto instance = engine_->instance();
  auto device = engine_->device();

  vkWaitForFences(device, static_cast<uint32_t>(renderFinishedFences_.size()),
                  renderFinishedFences_.data(), VK_TRUE, UINT64_MAX);

  for (auto semaphore : imageAcquiredSemaphores_)
    vkDestroySemaphore(device, semaphore, nullptr);

  for (auto semaphore : renderFinishedSemaphores_)
    vkDestroySemaphore(device, semaphore, nullptr);

  for (auto fence : renderFinishedFences_)
    vkDestroyFence(device, fence, nullptr);

  vkDestroyCommandPool(device, commandPool_, nullptr);

  destroyImageViews();
  vkDestroySwapchainKHR(device, swapchain_, nullptr);
  vkDestroySurfaceKHR(instance, surface_, nullptr);
}

bool Swapchain::resize(uint32_t width, uint32_t height) {
  if (swapchainInfo_.imageExtent.width != width ||
      swapchainInfo_.imageExtent.height != height) {
    auto device = engine_->device();

    vkWaitForFences(device, static_cast<uint32_t>(renderFinishedFences_.size()),
                    renderFinishedFences_.data(), VK_TRUE, UINT64_MAX);

    destroyImageViews();

    swapchainInfo_.imageExtent = {width, height};
    swapchainInfo_.oldSwapchain = swapchain_;

    VkSwapchainKHR newSwapchain;
    vkCreateSwapchainKHR(device, &swapchainInfo_, nullptr, &newSwapchain);
    if (swapchain_) vkDestroySwapchainKHR(device, swapchain_, nullptr);
    swapchain_ = newSwapchain;

    createImageViews();

    return true;
  }

  return false;
}

bool Swapchain::begin() {
  auto device = engine_->device();
  auto frameIndex = frameIndex_ % MAX_FRAMES_IN_FLIGHT;

  vkWaitForFences(device, 1, &renderFinishedFences_[frameIndex], VK_TRUE,
                  UINT64_MAX);
  vkResetFences(device, 1, &renderFinishedFences_[frameIndex]);

  auto result = vkAcquireNextImageKHR(device, swapchain_, UINT64_MAX,
                                      imageAcquiredSemaphores_[frameIndex],
                                      nullptr, &imageIndex_);

  switch (result) {
      // Success
    case VK_SUBOPTIMAL_KHR:
      std::cout << "Subobtimal next image" << std::endl;
      // Fall through
    case VK_SUCCESS:
      vkResetCommandBuffer(commandBuffers_[imageIndex_], 0);
      return true;

      // Failure
    default:
      return false;
  }
}

void Swapchain::end() {
  auto queue = engine_->queue();
  auto frameIndex = frameIndex_ % MAX_FRAMES_IN_FLIGHT;

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
  submitInfo.pSignalSemaphores = &renderFinishedSemaphores_[frameIndex];
  vkQueueSubmit(queue, 1, &submitInfo, renderFinishedFences_[frameIndex]);

  VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderFinishedSemaphores_[frameIndex];
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

void Swapchain::createImageViews() {
  auto device = engine_->device();

  vkGetSwapchainImagesKHR(device, swapchain_, &imageCount_, nullptr);
  images_.resize(imageCount_);
  vkGetSwapchainImagesKHR(device, swapchain_, &imageCount_, images_.data());

  VkImageViewCreateInfo imageViewInfo = {
      VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewInfo.format = swapchainInfo_.imageFormat;
  imageViewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

  imageViews_.resize(imageCount_);
  for (int i = 0; i < images_.size(); i++) {
    imageViewInfo.image = images_[i];
    vkCreateImageView(device, &imageViewInfo, nullptr, &imageViews_[i]);
  }
}

void Swapchain::destroyImageViews() {
  auto device = engine_->device();

  for (auto imageView : imageViews_)
    vkDestroyImageView(device, imageView, nullptr);
  images_.clear();
  imageViews_.clear();
}

}  // namespace instarf
