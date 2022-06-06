#ifndef INSTARF_FRAMEBUFFER_H
#define INSTARF_FRAMEBUFFER_H

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace instarf {

class Engine;

struct FramebufferCreateInfo {
  struct ImageInfo {
    VkImageUsageFlags usage;
    VkFormat format;
  };
  std::vector<ImageInfo> imageInfos;
};

class Framebuffer {
public:
  Framebuffer() = default;
  Framebuffer(Engine engine, const FramebufferCreateInfo& createInfo);
  ~Framebuffer() = default;

  void resize(uint32_t width, uint32_t height);

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace instarf

#endif  // INSTARF_FRAMEBUFFER_H
