#include <instarf/application.h>

#include <iostream>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <instarf/engine.h>
#include <instarf/swapchain.h>
#include <instarf/render_pass.h>

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

  EngineCreateInfo engineInfo;
  engineInfo.instanceExtensions = std::vector<std::string>(
      instanceExtensions, instanceExtensions + instanceExtensionCount);
  Engine engine(engineInfo);

  auto instance = engine.instance();
  VkSurfaceKHR surface;
  glfwCreateWindowSurface(instance, window_, nullptr, &surface);

  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);
  Swapchain swapchain(engine, surface);

  RenderPass renderPass(engine);

  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();

    glfwGetFramebufferSize(window_, &width, &height);

    // Minimized
    if (width == 0 || height == 0) continue;

    if (swapchain.resize(width, height)) {
      // TODO: update swapchain-dependent resources
    }

    if (swapchain.begin()) {
      auto cb = swapchain.commandBuffer();
      auto image = swapchain.image();

      VkCommandBufferBeginInfo beginInfo = {
          VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
      vkBeginCommandBuffer(cb, &beginInfo);

      // TODO: draw
      VkImageMemoryBarrier2 barrier = {
          VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
      barrier.srcStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
      barrier.srcAccessMask = 0;
      barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
      barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
      barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.image = image;
      barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

      VkDependencyInfo dependency = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
      dependency.imageMemoryBarrierCount = 1;
      dependency.pImageMemoryBarriers = &barrier;
      vkCmdPipelineBarrier2(cb, &dependency);

      VkClearColorValue clearColor;
      clearColor.float32[0] = 1.f;
      clearColor.float32[1] = 0.f;
      clearColor.float32[2] = 0.f;
      clearColor.float32[3] = 1.f;
      VkImageSubresourceRange range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
      vkCmdClearColorImage(cb, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           &clearColor, 1, &range);

      barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
      barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
      barrier.dstStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
      barrier.dstAccessMask = 0;
      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      vkCmdPipelineBarrier2(cb, &dependency);

      vkEndCommandBuffer(cb);

      swapchain.end();
    } else {
      std::cout << "Swapchain lost" << std::endl;
    }
  }

  glfwDestroyWindow(window_);
}

}  // namespace instarf
