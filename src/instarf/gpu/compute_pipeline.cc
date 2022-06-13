#include <instarf/gpu/compute_pipeline.h>

#include <instarf/gpu/device.h>
#include <instarf/gpu/detail/shader_module.h>

namespace instarf {
namespace gpu {

class ComputePipeline::Impl {
public:
  Impl() = delete;

  Impl(Device device, const ComputePipelineInfo& createInfo) : device_(device) {
    VkPipelineShaderStageCreateInfo stage = {
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    stage.module = createShaderModule(
        device, createInfo.directory + "/" + createInfo.name + ".vert.spv");
    stage.pName = "main";

    VkComputePipelineCreateInfo computePipelineInfo = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    computePipelineInfo.stage = stage;
    computePipelineInfo.layout = createInfo.layout;
    vkCreateComputePipelines(device, nullptr, 1, &computePipelineInfo, nullptr,
                             &pipeline_);

    vkDestroyShaderModule(device, stage.module, nullptr);
  }

  ~Impl() { vkDestroyPipeline(device_, pipeline_, nullptr); }

  operator VkPipeline() const noexcept { return pipeline_; }

private:
  Device device_;

  VkPipeline pipeline_;
};

ComputePipeline::ComputePipeline(Device device,
                                 const ComputePipelineInfo& createInfo)
    : impl_(std::make_shared<Impl>(device, createInfo)) {}

ComputePipeline::operator VkPipeline() const { return *impl_; }

}  // namespace gpu
}  // namespace instarf
