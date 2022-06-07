#include <instarf/compute_pipeline.h>

#include <instarf/engine.h>
#include <instarf/detail/shader_module.h>

namespace instarf {

class ComputePipeline::Impl {
public:
  Impl() = delete;

  Impl(Engine engine, const ComputePipelineInfo& createInfo) : engine_(engine) {
    auto device = engine.device();

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

  ~Impl() {
    auto device = engine_.device();
    vkDestroyPipeline(device, pipeline_, nullptr);
  }

  operator VkPipeline() const noexcept { return pipeline_; }

private:
  Engine engine_;

  VkPipeline pipeline_;
};

ComputePipeline::ComputePipeline(Engine engine,
                                 const ComputePipelineInfo& createInfo)
    : impl_(std::make_shared<Impl>(engine, createInfo)) {}

ComputePipeline::operator VkPipeline() const { return *impl_; }

}  // namespace instarf
