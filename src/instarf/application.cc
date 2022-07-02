#include <instarf/application.h>

#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <instarf/camera.h>
#include <instarf/gpu/instance.h>
#include <instarf/gpu/device.h>
#include <instarf/gpu/swapchain.h>
#include <instarf/gpu/attachment.h>
#include <instarf/gpu/render_pass.h>
#include <instarf/gpu/render_pass_ui.h>
#include <instarf/gpu/framebuffer.h>
#include <instarf/gpu/descriptor_layout.h>
#include <instarf/gpu/pipeline_layout.h>
#include <instarf/gpu/graphics_pipeline.h>
#include <instarf/gpu/descriptor.h>
#include <instarf/gpu/uniform_buffer.h>
#include <instarf/gpu/buffer.h>
#include <instarf/shader/camera_ubo.h>

namespace instarf {
namespace {
void checkVkResult(VkResult err) {
  if (err == 0) return;
  std::cerr << "[vulkan] Error: VkResult = err" << std::endl;
  if (err < 0) abort();
}
}  // namespace

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

  gpu::InstanceInfo instanceInfo;
  instanceInfo.extensions = std::vector<std::string>(
      instanceExtensions, instanceExtensions + instanceExtensionCount);
  gpu::Instance instance(instanceInfo);

  VkSurfaceKHR surface;
  glfwCreateWindowSurface(instance, window_, nullptr, &surface);

  gpu::DeviceInfo deviceInfo;
  gpu::Device device(instance, deviceInfo);

  gpu::Swapchain swapchain(device, surface);
  gpu::Attachment colorAttachment(device, VK_FORMAT_B8G8R8A8_UNORM,
                                  VK_SAMPLE_COUNT_4_BIT);
  gpu::Attachment depthAttachment(device, VK_FORMAT_D32_SFLOAT,
                                  VK_SAMPLE_COUNT_4_BIT);

  gpu::RenderPass renderPass(device);
  gpu::RenderPassUi renderPassUi(device);

  gpu::FramebufferInfo framebufferInfo;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.imageInfos = {
      {colorAttachment.usage(), colorAttachment.format()},
      {depthAttachment.usage(), depthAttachment.format()},
      {swapchain.imageUsage(), swapchain.format()},
  };
  gpu::Framebuffer framebuffer(device, framebufferInfo);

  framebufferInfo.renderPass = renderPassUi;
  framebufferInfo.imageInfos = {
      {swapchain.imageUsage(), swapchain.format()},
  };
  gpu::Framebuffer framebufferUi(device, framebufferInfo);

  gpu::DescriptorLayoutInfo descriptorLayoutInfo;
  descriptorLayoutInfo.bindings = {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
       VK_SHADER_STAGE_VERTEX_BIT},
  };
  gpu::DescriptorLayout cameraLayout(device, descriptorLayoutInfo);

  gpu::PipelineLayoutInfo pipelineLayoutInfo;
  pipelineLayoutInfo.layouts = {
      cameraLayout,
  };
  pipelineLayoutInfo.pushConstantRanges = {
      {VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4)},
  };
  gpu::PipelineLayout pipelineLayout(device, pipelineLayoutInfo);

  gpu::GraphicsPipelineInfo pipelineInfo;
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
  gpu::GraphicsPipeline colorPipeline(device, pipelineInfo);

  gpu::UniformBuffer<CameraUbo> cameraBuffer(device, swapchain.imageCount());

  gpu::Descriptor cameraDescriptor(device, cameraLayout);
  cameraDescriptor.bind(0, cameraBuffer);

  gpu::VertexBuffer<float> linesVertex(
      device, {0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
               0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
               0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f});
  gpu::IndexBuffer linesIndex(device, {0, 1, 2, 3, 4, 5});

  // ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();

  // Tweak WindowRounding/WindowBg so platform windows can look identical to
  // regular ones
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;
  style.Colors[ImGuiCol_WindowBg].w = 1.0f;

  ImGui_ImplGlfw_InitForVulkan(window_, true);
  ImGui_ImplVulkan_InitInfo initInfo = {};
  initInfo.Instance = instance;
  initInfo.PhysicalDevice = device.physicalDevice();
  initInfo.Device = device;
  initInfo.QueueFamily = device.queueIndex();
  initInfo.Queue = device.queue();
  initInfo.PipelineCache = nullptr;
  initInfo.DescriptorPool = device.descriptorPool();
  initInfo.Allocator = nullptr;
  initInfo.MinImageCount = swapchain.imageCount();
  initInfo.ImageCount = swapchain.imageCount();
  initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  initInfo.Subpass = 0;
  initInfo.CheckVkResultFn = checkVkResult;
  ImGui_ImplVulkan_Init(&initInfo, renderPassUi);

  // Upload Fonts
  device.submit([](VkCommandBuffer cb) {
    VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cb, &beginInfo);
    ImGui_ImplVulkan_CreateFontsTexture(cb);
    vkEndCommandBuffer(cb);
  });

  device.waitIdle();
  ImGui_ImplVulkan_DestroyFontUploadObjects();

  Camera camera;

  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();

    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse) {
      if (io.MouseDown[0] && !io.MouseDown[1])
        camera.rotateByPixels(io.MouseDelta.x, io.MouseDelta.y);
      else if (!io.MouseDown[0] && io.MouseDown[1])
        camera.translateByPixels(io.MouseDelta.x, io.MouseDelta.y);
      camera.zoomByScroll(io.MouseWheel);
    }

    if (!io.WantCaptureKeyboard) {
      constexpr float movingSpeed = 1.f;
      auto dx = camera.radius() * io.DeltaTime * movingSpeed;
      if (io.KeysDown['W']) camera.moveForward(dx);
      if (io.KeysDown['A']) camera.moveRight(-dx);
      if (io.KeysDown['S']) camera.moveForward(-dx);
      if (io.KeysDown['D']) camera.moveRight(dx);
      if (io.KeysDown[' ']) camera.moveUp(dx);
    }

    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);

    // Minimized
    if (width == 0 || height == 0) continue;

    camera.setScreenSize(width, height);

    if (swapchain.resize(width, height)) {
      colorAttachment.resize(width, height);
      depthAttachment.resize(width, height);
      framebuffer.resize(width, height);
      framebufferUi.resize(width, height);
    }

    if (swapchain.begin()) {
      // Start the Dear ImGui frame
      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      if (ImGui::Begin("instarf")) {
        ImGui::Text("FPS: %lf", io.Framerate);
      }
      ImGui::End();

      ImGui::Render();

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
      clearValues[0].color.float32[0] = 0.75f;
      clearValues[0].color.float32[1] = 0.75f;
      clearValues[0].color.float32[2] = 0.75f;
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

      glm::mat4 flip(1.f);
      flip[1][1] = -1.f;
      cameraBuffer[imageIndex].projection = flip * camera.projection();
      cameraBuffer[imageIndex].view = camera.view();

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

      std::vector<VkBuffer> vertexBuffers = {linesVertex};
      std::vector<VkDeviceSize> vertexOffsets = {0};
      vkCmdBindVertexBuffers(cb, 0, vertexBuffers.size(), vertexBuffers.data(),
                             vertexOffsets.data());
      vkCmdBindIndexBuffer(cb, linesIndex, 0, VK_INDEX_TYPE_UINT32);
      vkCmdDrawIndexed(cb, linesIndex.size(), 1, 0, 0, 0);

      vkCmdEndRenderPass(cb);

      // UI render pass
      attachments = {swapchain.imageView()};
      renderPassAttachments.attachmentCount = attachments.size();
      renderPassAttachments.pAttachments = attachments.data();
      renderPassBeginInfo.renderPass = renderPassUi;
      renderPassBeginInfo.framebuffer = framebufferUi;
      renderPassBeginInfo.clearValueCount = 0;
      vkCmdBeginRenderPass(cb, &renderPassBeginInfo,
                           VK_SUBPASS_CONTENTS_INLINE);
      ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb);
      vkCmdEndRenderPass(cb);

      vkEndCommandBuffer(cb);

      swapchain.end();
    } else {
      std::cout << "Swapchain lost" << std::endl;
    }

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  device.waitIdle();

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
}

}  // namespace instarf
