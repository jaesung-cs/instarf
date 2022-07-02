#include <instarf/gpu/compute_pipeline.h>

#include <instarf/gpu/device.h>
#include <instarf/gpu/detail/shader_module.h>

namespace instarf {
namespace gpu {

ComputePipeline::ComputePipeline(const Device& device, const ComputePipelineInfo& createInfo) : device_(device) {
  VkPipelineShaderStageCreateInfo stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
  stage.module = createShaderModule(device_, createInfo.directory + "/" + createInfo.name + ".vert.spv");
  stage.pName = "main";

  VkComputePipelineCreateInfo computePipelineInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
  computePipelineInfo.stage = stage;
  computePipelineInfo.layout = createInfo.layout;
  vkCreateComputePipelines(device_, nullptr, 1, &computePipelineInfo, nullptr, &pipeline_);

  vkDestroyShaderModule(device_, stage.module, nullptr);
}

ComputePipeline::~ComputePipeline() { vkDestroyPipeline(device_, pipeline_, nullptr); }

}  // namespace gpu
}  // namespace instarf
