#include <instarf/swapchain.h>

#include <instarf/engine.h>

namespace instarf {

Swapchain::Swapchain(const Engine& engine, VkSurfaceKHR surface)
    : engine_(&engine) {
  auto device = engine_->device();

  surface_ = surface;

  swapchainInfo_ = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  swapchainInfo_.surface = surface_;
  swapchainInfo_.minImageCount = 3;
  swapchainInfo_.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
  swapchainInfo_.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  swapchainInfo_.imageExtent = {0, 0};
  swapchainInfo_.imageArrayLayers = 1;
  swapchainInfo_.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainInfo_.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapchainInfo_.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainInfo_.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  swapchainInfo_.clipped = true;
}

Swapchain::~Swapchain() {
  auto instance = engine_->instance();
  auto device = engine_->device();

  destroyImageViews();
  vkDestroySwapchainKHR(device, swapchain_, nullptr);
  vkDestroySurfaceKHR(instance, surface_, nullptr);
}

bool Swapchain::resize(uint32_t width, uint32_t height) {
  if (swapchainInfo_.imageExtent.width != width ||
      swapchainInfo_.imageExtent.height != height) {
    auto device = engine_->device();

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
