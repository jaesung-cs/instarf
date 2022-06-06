#include <instarf/application.h>

#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include <instarf/engine.h>
#include <instarf/swapchain.h>
#include <instarf/attachment.h>
#include <instarf/render_pass.h>
#include <instarf/framebuffer.h>
#include <instarf/descriptor_layout.h>
#include <instarf/pipeline_layout.h>
#include <instarf/graphics_pipeline.h>
#include <instarf/descriptor.h>
#include <instarf/uniform_buffer.h>
#include <instarf/shader/camera_ubo.h>

namespace instarf {

Application::Application() {
  if (glfwInit() != GLFW_TRUE) throw std::runtime_error("Failed to init glfw");
}

Application::~Application() { glfwTerminate(); }

void Application::run() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  width_ = 1600;
  height_ = 900;
  window_ = glfwCreateWindow(width_, height_, "instarf", NULL, NULL);
  if (window_ == NULL) throw std::runtime_error("Failed to create window");

  uint32_t instanceExtensionCount;
  const char** instanceExtensions =
      glfwGetRequiredInstanceExtensions(&instanceExtensionCount);

  EngineInfo engineInfo;
  engineInfo.instanceExtensions = std::vector<std::string>(
      instanceExtensions, instanceExtensions + instanceExtensionCount);
  Engine engine(engineInfo);

  auto instance = engine.instance();
  VkSurfaceKHR surface;
  glfwCreateWindowSurface(instance, window_, nullptr, &surface);

  Swapchain swapchain(engine, surface);
  Attachment colorAttachment(engine, VK_FORMAT_B8G8R8A8_UNORM,
                             VK_SAMPLE_COUNT_4_BIT);
  Attachment depthAttachment(engine, VK_FORMAT_D32_SFLOAT,
                             VK_SAMPLE_COUNT_4_BIT);

  RenderPass renderPass(engine);

  FramebufferInfo framebufferInfo;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.imageInfos = {
      {colorAttachment.usage(), colorAttachment.format()},
      {depthAttachment.usage(), depthAttachment.format()},
      {swapchain.imageUsage(), swapchain.format()},
  };
  Framebuffer framebuffer(engine, framebufferInfo);

  DescriptorLayoutInfo descriptorLayoutInfo;
  descriptorLayoutInfo.bindings = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
       VK_SHADER_STAGE_VERTEX_BIT},
  };
  DescriptorLayout cameraLayout(engine, descriptorLayoutInfo);

  PipelineLayoutInfo pipelineLayoutInfo;
  pipelineLayoutInfo.layouts = {
      cameraLayout,
  };
  pipelineLayoutInfo.pushConstantRanges = {
      {VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4)},
  };
  PipelineLayout pipelineLayout(engine, pipelineLayoutInfo);

  GraphicsPipelineInfo pipelineInfo;
  pipelineInfo.directory = "C:\\workspace\\instarf\\assets\\shaders";
  pipelineInfo.name = "color";
  pipelineInfo.bindings = {
      {0, sizeof(float) * 6, VK_VERTEX_INPUT_RATE_VERTEX},
  };
  pipelineInfo.attributes = {
      {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
      {1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3},
  };
  pipelineInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  pipelineInfo.samples = VK_SAMPLE_COUNT_4_BIT;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  GraphicsPipeline colorPipeline(engine, pipelineInfo);

  UniformBuffer<CameraUbo> cameraBuffer(engine, swapchain.imageCount());

  Descriptor cameraDescriptor(engine, cameraLayout);
  cameraDescriptor.bind(0, cameraBuffer);

  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);

    // Minimized
    if (width == 0 || height == 0) continue;

    if (swapchain.resize(width, height)) {
      colorAttachment.resize(width, height);
      depthAttachment.resize(width, height);
      framebuffer.resize(width, height);
    }

    if (swapchain.begin()) {
      auto cb = swapchain.commandBuffer();
      auto imageIndex = swapchain.imageIndex();

      VkCommandBufferBeginInfo beginInfo = {
          VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
      vkBeginCommandBuffer(cb, &beginInfo);

      VkViewport viewport = {
          0.f, 0.f, static_cast<float>(width), static_cast<float>(height),
          0.f, 1.f};
      VkRect2D scissor = {
          {0, 0},
          {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}};

      std::vector<VkClearValue> clearValues(2);
      clearValues[0].color.float32[0] = 1.f;
      clearValues[0].color.float32[1] = 0.f;
      clearValues[0].color.float32[2] = 0.f;
      clearValues[0].color.float32[3] = 1.f;
      clearValues[1].depthStencil.depth = 1.f;
      clearValues[1].depthStencil.stencil = 0;

      std::vector<VkImageView> attachments = {colorAttachment, depthAttachment,
                                              swapchain.imageView()};
      VkRenderPassAttachmentBeginInfo renderPassAttachments = {
          VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO};
      renderPassAttachments.attachmentCount = attachments.size();
      renderPassAttachments.pAttachments = attachments.data();

      VkRenderPassBeginInfo renderPassBeginInfo = {
          VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
      renderPassBeginInfo.pNext = &renderPassAttachments;
      renderPassBeginInfo.renderPass = renderPass;
      renderPassBeginInfo.framebuffer = framebuffer;
      renderPassBeginInfo.renderArea = scissor;
      renderPassBeginInfo.clearValueCount = clearValues.size();
      renderPassBeginInfo.pClearValues = clearValues.data();
      vkCmdBeginRenderPass(cb, &renderPassBeginInfo,
                           VK_SUBPASS_CONTENTS_INLINE);

      vkCmdSetViewport(cb, 0, 1, &viewport);
      vkCmdSetScissor(cb, 0, 1, &scissor);

      vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, colorPipeline);

      cameraBuffer[imageIndex].projection = glm::mat4(1.f);
      cameraBuffer[imageIndex].view = glm::mat4(1.f);

      std::vector<VkDescriptorSet> descriptors = {cameraDescriptor};
      std::vector<uint32_t> offsets = {cameraBuffer.offset(imageIndex)};
      vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelineLayout, 0, descriptors.size(),
                              descriptors.data(), offsets.size(),
                              offsets.data());

      struct ModelPush {
        glm::mat4 model;
      } model;
      model.model = glm::mat4(1.f);
      vkCmdPushConstants(cb, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                         sizeof(model), &model);

      vkCmdEndRenderPass(cb);

      vkEndCommandBuffer(cb);

      swapchain.end();
    } else {
      std::cout << "Swapchain lost" << std::endl;
    }
  }

  engine.waitIdle();
  glfwDestroyWindow(window_);
}

}  // namespace instarf
