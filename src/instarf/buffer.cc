#include <instarf/buffer.h>

#include "vk_mem_alloc.h"

namespace instarf {

class BufferBase::Impl {
public:
  Impl() = delete;

  Impl(Device device, VkBufferUsageFlags usage, VkDeviceSize size,
       const void* ptr)
      : device_(device) {
    auto allocator = device.allocator();

    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = size;
    bufferInfo.usage = usage;

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

    vmaCreateBuffer(allocator, &bufferInfo, &allocationCreateInfo, &buffer_,
                    &allocation_, nullptr);

    if (ptr) {
      bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

      allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                   VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

      VmaAllocationInfo allocationInfo;
      vmaCreateBuffer(allocator, &bufferInfo, &allocationCreateInfo,
                      &stagingBuffer_, &stagingAllocation_, &allocationInfo);

      std::memcpy(allocationInfo.pMappedData, ptr, size);

      device.submit([this, size](VkCommandBuffer cb) {
        VkCommandBufferBeginInfo beginInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cb, &beginInfo);

        VkBufferCopy region = {0, 0, size};
        vkCmdCopyBuffer(cb, stagingBuffer_, buffer_, 1, &region);

        VkBufferMemoryBarrier2 barrier = {
            VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2};
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
        barrier.buffer = buffer_;
        barrier.offset = 0;
        barrier.size = size;

        VkDependencyInfo dependency = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
        dependency.bufferMemoryBarrierCount = 1;
        dependency.pBufferMemoryBarriers = &barrier;
        vkCmdPipelineBarrier2(cb, &dependency);

        vkEndCommandBuffer(cb);
      });
    }
  }

  ~Impl() {
    auto allocator = device_.allocator();

    vmaDestroyBuffer(allocator, buffer_, allocation_);

    if (stagingBuffer_)
      vmaDestroyBuffer(allocator, stagingBuffer_, stagingAllocation_);
  }

  operator VkBuffer() const { return buffer_; }

private:
  Device device_;

  VkBuffer buffer_;
  VmaAllocation allocation_;

  VkBuffer stagingBuffer_ = VK_NULL_HANDLE;
  VmaAllocation stagingAllocation_;
};

BufferBase::BufferBase(Device device, VkBufferUsageFlags usage,
                       VkDeviceSize size, const void* ptr)
    : impl_(std::make_shared<Impl>(device, usage, size, ptr)) {}

BufferBase::operator VkBuffer() const { return *impl_; }

}  // namespace instarf
