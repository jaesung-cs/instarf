#include <instarf/gpu/device.h>

#include <instarf/gpu/instance.h>

namespace instarf {
namespace gpu {

class Device::Impl {
public:
  Impl() = delete;

  Impl(Instance instance, const DeviceInfo& createInfo) {
    // Physical device
    // TODO: match with UUID
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
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
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(
        queueFamilyCount);
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

    // Allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    allocatorInfo.physicalDevice = physicalDevice_;
    allocatorInfo.device = device_;
    allocatorInfo.instance = instance;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    vmaCreateAllocator(&allocatorInfo, &allocator_);

    // Descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024},
    };
    VkDescriptorPoolCreateInfo descriptorPoolInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    descriptorPoolInfo.flags =
        VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolInfo.maxSets = 1024;
    descriptorPoolInfo.poolSizeCount = poolSizes.size();
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    vkCreateDescriptorPool(device_, &descriptorPoolInfo, nullptr,
                           &descriptorPool_);

    // Command pool
    VkCommandPoolCreateInfo commandPoolInfo = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    commandPoolInfo.queueFamilyIndex = queueIndex_;
    vkCreateCommandPool(device_, &commandPoolInfo, nullptr, &commandPool_);
  }

  ~Impl() {
    vkDestroyCommandPool(device_, commandPool_, nullptr);
    vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);
    vmaDestroyAllocator(allocator_);
    vkDestroyDevice(device_, nullptr);
  }

  operator VkDevice() const noexcept { return device_; }

  auto physicalDevice() const noexcept { return physicalDevice_; }
  auto queueIndex() const noexcept { return queueIndex_; }
  auto queue() const noexcept { return queue_; }
  auto allocator() const noexcept { return allocator_; }
  auto descriptorPool() const noexcept { return descriptorPool_; }

  void submit(CommandRecordFunc command) {
    // TODO: reuse command buffer
    VkCommandBuffer cb;

    VkCommandBufferAllocateInfo commandBufferInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    commandBufferInfo.commandPool = commandPool_;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(device_, &commandBufferInfo, &cb);

    command(cb);

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cb;
    vkQueueSubmit(queue_, 1, &submitInfo, nullptr);
  }

  void waitIdle() { vkDeviceWaitIdle(device_); }

private:
  VkPhysicalDevice physicalDevice_;
  VkDevice device_;
  int queueIndex_ = -1;
  VkQueue queue_;

  VmaAllocator allocator_;
  VkDescriptorPool descriptorPool_;
  VkCommandPool commandPool_;
};

Device::Device(Instance instance, const DeviceInfo& createInfo)
    : impl_(std::make_shared<Impl>(instance, createInfo)) {}

Device::operator VkDevice() const { return *impl_; }

VkPhysicalDevice Device::physicalDevice() const {
  return impl_->physicalDevice();
}
int Device::queueIndex() const { return impl_->queueIndex(); }
VkQueue Device::queue() const { return impl_->queue(); }
VmaAllocator Device::allocator() const { return impl_->allocator(); }
VkDescriptorPool Device::descriptorPool() const {
  return impl_->descriptorPool();
}

void Device::submit(CommandRecordFunc command) { impl_->submit(command); }
void Device::waitIdle() { impl_->waitIdle(); }

}  // namespace gpu
}  // namespace instarf
