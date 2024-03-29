#include <instarf/gpu/attachment.h>

#include "vk_mem_alloc.h"

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

class Attachment::Impl {
public:
  Impl() = delete;

  Impl(Device device, VkFormat format, VkSampleCountFlagBits samples)
      : device_(device) {
    VkImageUsageFlags usage = 0;
    VkImageAspectFlags aspect = 0;

    switch (format) {
      case VK_FORMAT_D16_UNORM:
      case VK_FORMAT_D16_UNORM_S8_UINT:
      case VK_FORMAT_D24_UNORM_S8_UINT:
      case VK_FORMAT_D32_SFLOAT_S8_UINT:
      case VK_FORMAT_D32_SFLOAT:
        usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
        break;

      default:
        usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    imageInfo_ = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo_.imageType = VK_IMAGE_TYPE_2D;
    imageInfo_.format = format;
    imageInfo_.mipLevels = 1;
    imageInfo_.arrayLayers = 1;
    imageInfo_.samples = samples;
    imageInfo_.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo_.usage = usage;
    imageInfo_.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    allocationInfo_ = {};
    allocationInfo_.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocationInfo_.usage = VMA_MEMORY_USAGE_AUTO;

    imageViewInfo_ = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    imageViewInfo_.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo_.format = format;
    imageViewInfo_.subresourceRange = {aspect, 0, 1, 0, 1};
  }

  ~Impl() {
    if (image_) {
      auto allocator = device_.allocator();

      vmaDestroyImage(allocator, image_, allocation_);
      vkDestroyImageView(device_, imageView_, nullptr);
    }
  }

  operator VkImageView() const noexcept { return imageView_; }
  auto usage() const noexcept { return imageInfo_.usage; }
  auto format() const noexcept { return imageInfo_.format; }

  void resize(uint32_t width, uint32_t height) {
    if (imageInfo_.extent.width != width ||
        imageInfo_.extent.height != height) {
      auto allocator = device_.allocator();

      if (image_) {
        vmaDestroyImage(allocator, image_, allocation_);
        vkDestroyImageView(device_, imageView_, nullptr);
      }

      imageInfo_.extent = {width, height, 1};
      vmaCreateImage(allocator, &imageInfo_, &allocationInfo_, &image_,
                     &allocation_, nullptr);

      imageViewInfo_.image = image_;
      vkCreateImageView(device_, &imageViewInfo_, nullptr, &imageView_);
    }
  }

private:
  Device device_;

  VkImageCreateInfo imageInfo_;
  VkImage image_ = VK_NULL_HANDLE;
  VmaAllocationCreateInfo allocationInfo_;
  VmaAllocation allocation_;
  VkImageViewCreateInfo imageViewInfo_;
  VkImageView imageView_ = VK_NULL_HANDLE;
};

Attachment::Attachment(Device device, VkFormat format,
                       VkSampleCountFlagBits samples)
    : impl_(std::make_shared<Impl>(device, format, samples)) {}

Attachment::operator VkImageView() const { return *impl_; }

VkImageUsageFlags Attachment::usage() const { return impl_->usage(); }
VkFormat Attachment::format() const { return impl_->format(); }

void Attachment::resize(uint32_t width, uint32_t height) {
  impl_->resize(width, height);
}

}  // namespace gpu
}  // namespace instarf
