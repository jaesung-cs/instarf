#include <instarf/gpu/pipeline_layout.h>

#include <instarf/gpu/device.h>

namespace instarf {
namespace gpu {

class PipelineLayout::Impl {
public:
  Impl() = delete;

  Impl(Device device, const PipelineLayoutInfo& createInfo) : device_(device) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.setLayoutCount = createInfo.layouts.size();
    pipelineLayoutInfo.pSetLayouts = createInfo.layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount =
        createInfo.pushConstantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges =
        createInfo.pushConstantRanges.data();
    vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &layout_);
  }

  ~Impl() { vkDestroyPipelineLayout(device_, layout_, nullptr); }

  operator VkPipelineLayout() const noexcept { return layout_; }

private:
  Device device_;

  VkPipelineLayout layout_;
};

PipelineLayout::PipelineLayout(Device device,
                               const PipelineLayoutInfo& createInfo)
    : impl_(std::make_shared<Impl>(device, createInfo)) {}

PipelineLayout::operator VkPipelineLayout() const { return *impl_; }

}  // namespace gpu
}  // namespace instarf
