#include <instarf/engine.h>

#include <iostream>
#include <vector>

namespace instarf {
namespace {
// Validation layer callback
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* pUserData) {
  std::cerr << callbackData->pMessage << std::endl << std::endl;

  return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr)
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  else
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) func(instance, debugMessenger, pAllocator);
}
}  // namespace

Engine::Engine(const EngineCreateInfo& engineInfo) {
  // Instance
  VkApplicationInfo applicationInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  applicationInfo.pApplicationName = "instarf viewer";
  applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.pEngineName = "instarf";
  applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.apiVersion = VK_API_VERSION_1_3;

  std::vector<const char*> instanceLayers;
  instanceLayers.push_back("VK_LAYER_KHRONOS_validation");

  std::vector<const char*> instanceExtensions;
  instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  for (const auto& extension : engineInfo.instanceExtensions)
    instanceExtensions.push_back(extension.c_str());

  VkInstanceCreateInfo instanceInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  instanceInfo.pApplicationInfo = &applicationInfo;
  instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
  instanceInfo.ppEnabledLayerNames = instanceLayers.data();
  instanceInfo.enabledExtensionCount =
      static_cast<uint32_t>(instanceExtensions.size());
  instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
  vkCreateInstance(&instanceInfo, nullptr, &instance_);

  VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
  messengerInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  messengerInfo.pfnUserCallback = debugCallback;
  messengerInfo.pUserData = nullptr;
  createDebugUtilsMessengerEXT(instance_, &messengerInfo, nullptr, &messenger_);

  // Physical device
  uint32_t physicalDeviceCount;
  vkEnumeratePhysicalDevices(instance_, &physicalDeviceCount, nullptr);
  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(instance_, &physicalDeviceCount,
                             physicalDevices.data());

  physicalDevice_ = physicalDevices[0];

  // Device
  VkPhysicalDeviceSynchronization2Features synchronization2 = {
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES};

  VkPhysicalDeviceImagelessFramebufferFeatures imagelessFramebuffer = {
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES};
  imagelessFramebuffer.pNext = &synchronization2;

  VkPhysicalDeviceFeatures2 features = {
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
  features.pNext = &imagelessFramebuffer;
  vkGetPhysicalDeviceFeatures2(physicalDevice_, &features);

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount,
                                           queueFamilyProperties.data());
  constexpr VkQueueFlags requiredQueueFlags =
      VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
  for (int i = 0; i < queueFamilyProperties.size(); i++) {
    const auto& properties = queueFamilyProperties[i];
    if ((properties.queueFlags & requiredQueueFlags) == requiredQueueFlags) {
      queueIndex_ = i;
      break;
    }
  }

  std::vector<float> priorities = {1.f};
  std::vector<VkDeviceQueueCreateInfo> queueInfos(1);
  queueInfos[0] = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  queueInfos[0].queueFamilyIndex = queueIndex_;
  queueInfos[0].queueCount = 1;
  queueInfos[0].pQueuePriorities = priorities.data();

  std::vector<const char*> deviceExtensions = {
      VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
      VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME,
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
      VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
  };

  VkDeviceCreateInfo deviceInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  deviceInfo.pNext = &features;
  deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
  deviceInfo.pQueueCreateInfos = queueInfos.data();
  deviceInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
  deviceInfo.pEnabledFeatures = NULL;
  vkCreateDevice(physicalDevice_, &deviceInfo, nullptr, &device_);

  vkGetDeviceQueue(device_, queueIndex_, 0, &queue_);
}

Engine::~Engine() {
  vkDestroyDevice(device_, nullptr);

  destroyDebugUtilsMessengerEXT(instance_, messenger_, nullptr);
  vkDestroyInstance(instance_, nullptr);
}

}  // namespace instarf
