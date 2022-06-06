#include <instarf/application.h>

#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <instarf/engine.h>
#include <instarf/swapchain.h>

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

  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();

    glfwGetFramebufferSize(window_, &width, &height);
    if (swapchain.resize(width, height)) {
      // TODO: update swapchain-dependent resources
    }

    // TODO: draw
  }

  glfwDestroyWindow(window_);
}

}  // namespace instarf
