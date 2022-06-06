#include <instarf/pipeline_layout.h>

#include <instarf/engine.h>

namespace instarf {

class PipelineLayout::Impl {
public:
  Impl() = delete;

  Impl(Engine engine, const PipelineLayoutInfo& createInfo) : engine_(engine) {
    auto device = engine_.device();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.setLayoutCount = createInfo.layouts.size();
    pipelineLayoutInfo.pSetLayouts = createInfo.layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount =
        createInfo.pushConstantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges =
        createInfo.pushConstantRanges.data();
    vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &layout_);
  }

  ~Impl() {
    auto device = engine_.device();
    vkDestroyPipelineLayout(device, layout_, nullptr);
  }

  operator VkPipelineLayout() const noexcept { return layout_; }

private:
  Engine engine_;

  VkPipelineLayout layout_;
};

PipelineLayout::PipelineLayout(Engine engine,
                               const PipelineLayoutInfo& createInfo)
    : impl_(std::make_shared<Impl>(engine, createInfo)) {}

PipelineLayout::operator VkPipelineLayout() const { return *impl_; }

}  // namespace instarf
