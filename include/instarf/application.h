#ifndef INSTARF_APPLICATION_H
#define INSTARF_APPLICATION_H

#include <cstdint>

struct GLFWwindow;

namespace instarf {

class Application {
public:
  Application();
  ~Application();

  void run();

private:
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  GLFWwindow* window_ = nullptr;
};

}  // namespace instarf

#endif  // INSTARF_APPLICATION_H
