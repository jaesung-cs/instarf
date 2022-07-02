#ifndef INSTARF_GPU_PIPELINE_LAYOUT_H
#define INSTARF_GPU_PIPELINE_LAYOUT_H

#include <vector>

#include <vulkan/vulkan.h>

namespace instarf {
namespace gpu {

class Device;

struct PipelineLayoutInfo {
  std::vector<VkDescriptorSetLayout> layouts;
  std::vector<VkPushConstantRange> pushConstantRanges;
};

class PipelineLayout {
public:
  PipelineLayout() = delete;
  PipelineLayout(const Device& device, const PipelineLayoutInfo& createInfo);
  ~PipelineLayout();

  operator VkPipelineLayout() const noexcept { return layout_; }

private:
  VkDevice device_ = VK_NULL_HANDLE;

  VkPipelineLayout layout_ = VK_NULL_HANDLE;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_PIPELINE_LAYOUT_H
