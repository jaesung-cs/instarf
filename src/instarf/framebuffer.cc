#include <instarf/framebuffer.h>

#include <instarf/engine.h>

namespace instarf {

class Framebuffer::Impl {
public:
  Impl() = delete;

  Impl(Engine engine, const FramebufferInfo& createInfo)
      : engine_(engine) {
    const auto& imageInfos = createInfo.imageInfos;
    formats_.resize(imageInfos.size());
    imageInfos_.resize(imageInfos.size());
    for (int i = 0; i < imageInfos.size(); i++) {
      formats_[i] = imageInfos[i].format;

      imageInfos_[i] = {VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO};
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
    framebufferInfo_.renderPass;
    framebufferInfo_.attachmentCount;
    framebufferInfo_.layers = 1;
  }

  ~Impl() {
    if (framebuffer_) {
      auto device = engine_.device();
      vkDestroyFramebuffer(device, framebuffer_, nullptr);
    }
  }

  void resize(uint32_t width, uint32_t height) {
    if (framebufferInfo_.width != width || framebufferInfo_.height != height) {
      auto device = engine_.device();

      for (auto& imageInfo : imageInfos_) {
        imageInfo.width = width;
        imageInfo.height = height;
      }
      framebufferInfo_.width = width;
      framebufferInfo_.height = height;

      if (framebuffer_) vkDestroyFramebuffer(device, framebuffer_, nullptr);
      vkCreateFramebuffer(device, &framebufferInfo_, nullptr, &framebuffer_);
    }
  }

private:
  Engine engine_;

  std::vector<VkFormat> formats_;
  std::vector<VkFramebufferAttachmentImageInfo> imageInfos_;
  VkFramebufferAttachmentsCreateInfo attachmentsInfo_;
  VkFramebufferCreateInfo framebufferInfo_;
  VkFramebuffer framebuffer_ = VK_NULL_HANDLE;
};

Framebuffer::Framebuffer(Engine engine, const FramebufferInfo& createInfo)
    : impl_(std::make_shared<Impl>(engine, createInfo)) {}

void Framebuffer::resize(uint32_t width, uint32_t height) {
  impl_->resize(width, height);
}

}  // namespace instarf
