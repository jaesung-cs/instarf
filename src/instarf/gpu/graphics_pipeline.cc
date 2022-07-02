#include <instarf/gpu/graphics_pipeline.h>

#include <instarf/gpu/device.h>
#include <instarf/gpu/detail/shader_module.h>

namespace instarf {
namespace gpu {

GraphicsPipeline::GraphicsPipeline(const Device& device, const GraphicsPipelineInfo& createInfo) : device_(device) {
  std::vector<VkPipelineShaderStageCreateInfo> stages(2);
  stages[0] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  stages[0].module = createShaderModule(device, createInfo.directory + "/" + createInfo.name + ".vert.spv");
  stages[0].pName = "main";

  stages[1] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  stages[1].module = createShaderModule(device, createInfo.directory + "/" + createInfo.name + ".frag.spv");
  stages[1].pName = "main";

  VkPipelineVertexInputStateCreateInfo vertexInputState = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  vertexInputState.vertexBindingDescriptionCount = createInfo.bindings.size();
  vertexInputState.pVertexBindingDescriptions = createInfo.bindings.data();
  vertexInputState.vertexAttributeDescriptionCount = createInfo.attributes.size();
  vertexInputState.pVertexAttributeDescriptions = createInfo.attributes.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
  inputAssemblyState.topology = createInfo.topology;

  VkViewport viewport;
  VkRect2D scissor;
  VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizationState = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
  rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationState.cullMode = VK_CULL_MODE_NONE;
  rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizationState.lineWidth = 1.f;

  VkPipelineMultisampleStateCreateInfo multisampleState = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
  multisampleState.rasterizationSamples = createInfo.samples;

  VkPipelineDepthStencilStateCreateInfo depthStencilState = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
  depthStencilState.depthTestEnable = VK_TRUE;
  depthStencilState.depthWriteEnable = VK_TRUE;
  depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlendState = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  colorBlendState.attachmentCount = 1;
  colorBlendState.pAttachments = &colorBlendAttachment;

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
  dynamicState.dynamicStateCount = dynamicStates.size();
  dynamicState.pDynamicStates = dynamicStates.data();

  VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
  graphicsPipelineInfo.stageCount = stages.size();
  graphicsPipelineInfo.pStages = stages.data();
  graphicsPipelineInfo.pVertexInputState = &vertexInputState;
  graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyState;
  graphicsPipelineInfo.pViewportState = &viewportState;
  graphicsPipelineInfo.pRasterizationState = &rasterizationState;
  graphicsPipelineInfo.pMultisampleState = &multisampleState;
  graphicsPipelineInfo.pDepthStencilState = &depthStencilState;
  graphicsPipelineInfo.pColorBlendState = &colorBlendState;
  graphicsPipelineInfo.pDynamicState = &dynamicState;
  graphicsPipelineInfo.layout = createInfo.layout;
  graphicsPipelineInfo.renderPass = createInfo.renderPass;
  graphicsPipelineInfo.subpass = createInfo.subpass;
  vkCreateGraphicsPipelines(device, nullptr, 1, &graphicsPipelineInfo, nullptr, &pipeline_);

  for (const auto& stage : stages) vkDestroyShaderModule(device, stage.module, nullptr);
}

GraphicsPipeline::~GraphicsPipeline() { vkDestroyPipeline(device_, pipeline_, nullptr); }

}  // namespace gpu
}  // namespace instarf
