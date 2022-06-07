#ifndef INSTARF_PIPELINE_LAYOUT_H
#define INSTARF_PIPELINE_LAYOUT_H

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace instarf {

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

}  // namespace instarf

#endif  // INSTARF_PIPELINE_LAYOUT_H
