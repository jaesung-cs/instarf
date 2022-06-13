#ifndef INSTARF_GPU_PIPELINE_LAYOUT_H
#define INSTARF_GPU_PIPELINE_LAYOUT_H

#include <memory>
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
  PipelineLayout() = default;
  PipelineLayout(Device device, const PipelineLayoutInfo& createInfo);
  ~PipelineLayout() = default;

  operator VkPipelineLayout() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace gpu
}  // namespace instarf

#endif  // INSTARF_GPU_PIPELINE_LAYOUT_H
