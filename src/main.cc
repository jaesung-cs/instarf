#include <iostream>

#include <instarf/application.h>

int main() {
  try {
    instarf::Application app;
    app.run();
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
