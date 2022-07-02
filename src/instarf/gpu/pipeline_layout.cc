#include <instarf/gpu/pipeline_layout.h>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

PipelineLayout::PipelineLayout(const Device& device, const PipelineLayoutInfo& createInfo) : device_(device) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipelineLayoutInfo.setLayoutCount = createInfo.layouts.size();
  pipelineLayoutInfo.pSetLayouts = createInfo.layouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = createInfo.pushConstantRanges.size();
  pipelineLayoutInfo.pPushConstantRanges = createInfo.pushConstantRanges.data();
  vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &layout_);
}

PipelineLayout::~PipelineLayout() { vkDestroyPipelineLayout(device_, layout_, nullptr); }

}  // namespace gpu
}  // namespace instarf
