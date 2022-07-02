#include <instarf/gpu/framebuffer.h>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

Framebuffer::Framebuffer(const Device& device, const FramebufferInfo& createInfo) : device_(device) {
  const auto& imageInfos = createInfo.imageInfos;
  formats_.resize(imageInfos.size());
  imageInfos_.resize(imageInfos.size());
  for (int i = 0; i < imageInfos.size(); i++) {
    formats_[i] = imageInfos[i].format;

    imageInfos_[i] = {VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO};
    imageInfos_[i].usage = imageInfos[i].usage;
    imageInfos_[i].layerCount = 1;
    imageInfos_[i].viewFormatCount = 1;
    imageInfos_[i].pViewFormats = &formats_[i];
  }

  attachmentsInfo_ = {VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO};
  attachmentsInfo_.attachmentImageInfoCount = imageInfos_.size();
  attachmentsInfo_.pAttachmentImageInfos = imageInfos_.data();

  framebufferInfo_ = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  framebufferInfo_.pNext = &attachmentsInfo_;
  framebufferInfo_.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
  framebufferInfo_.renderPass = createInfo.renderPass;
  framebufferInfo_.attachmentCount = imageInfos.size();
  framebufferInfo_.layers = 1;
}

Framebuffer::~Framebuffer() {
  if (framebuffer_) vkDestroyFramebuffer(device_, framebuffer_, nullptr);
}

void Framebuffer::resize(uint32_t width, uint32_t height) {
  if (framebufferInfo_.width != width || framebufferInfo_.height != height) {
    for (auto& imageInfo : imageInfos_) {
      imageInfo.width = width;
      imageInfo.height = height;
    }
    framebufferInfo_.width = width;
    framebufferInfo_.height = height;

    if (framebuffer_) vkDestroyFramebuffer(device_, framebuffer_, nullptr);
    vkCreateFramebuffer(device_, &framebufferInfo_, nullptr, &framebuffer_);
  }
}

}  // namespace gpu
}  // namespace instarf
