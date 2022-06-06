#include <instarf/application.h>

#include <stdexcept>

#include <GLFW/glfw3.h>

#include <instarf/engine.h>

namespace instarf {

Application::Application() {
  if (glfwInit() != GLFW_TRUE) throw std::runtime_error("Failed to init glfw");
}

Application::~Application() { glfwTerminate(); }

void Application::run() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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

  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();

    // TODO
  }

  glfwDestroyWindow(window_);
}

}  // namespace instarf
