#include <command.h>
#include <physicalDevice.h>
#include <stdio.h>

int createCommandPool(CommandPoolArgs *pArgs, VkCommandPool *outCommandPool) {

  QueueFamilyIndices queueFamilyIndices =
      findQueueFamilies(pArgs->pPhysicalDevice, pArgs->pSurface);

  VkCommandPoolCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .queueFamilyIndex = queueFamilyIndices.graphicsFamily,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT};

  if (vkCreateCommandPool(*pArgs->pDevice, &createInfo, NULL, outCommandPool) !=
      VK_SUCCESS) {
    fprintf(stderr, "Failed to create command pool\n");
    return 1;
  }
  return 0;
}

int createCommandBuffer(CommandBufferArgs *pArgs,
                        VkCommandBuffer *outCommandBuffer) {
  VkCommandBufferAllocateInfo allocateInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = *pArgs->pCommandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1};

  if (vkAllocateCommandBuffers(*pArgs->pDevice, &allocateInfo,
                               outCommandBuffer) != VK_SUCCESS) {
    fprintf(stderr, "Failed to allocate command buffer\n");
    return 1;
  }
  return 0;
}

int recordCommandBuffer(RecordCommandBufferArgs *pArgs) {
  VkCommandBufferBeginInfo beginInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
  };

  if (vkBeginCommandBuffer(*pArgs->pCommandBuffer, &beginInfo) != VK_SUCCESS) {
    fprintf(stderr, "Failed to begin recording command buffer\n");
    return 1;
  }

  VkOffset2D offset = {.x = 0, .y = 0};

  VkRect2D renderArea = {.extent = *pArgs->pSwapchainExtent, .offset = offset};

  VkClearValue clearValue = {.color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}}};

  VkRenderPassBeginInfo renderPassInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = *pArgs->pRenderPass,
      .framebuffer = pArgs->pSwapchainFramebuffers[pArgs->swapchainImageIndex],
      .renderArea = renderArea,
      .clearValueCount = 1,
      .pClearValues = &clearValue};

  vkCmdBeginRenderPass(*pArgs->pCommandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(*pArgs->pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    *pArgs->pGraphicsPipeline);

  VkViewport viewport = {
      .x = 0.0f,
      .y = 0.0f,
      .width = (float)pArgs->pSwapchainExtent->width,
      .height = (float)pArgs->pSwapchainExtent->width,
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  vkCmdSetViewport(*pArgs->pCommandBuffer, 0, 1, &viewport);

  VkRect2D scissor = {.extent = *pArgs->pSwapchainExtent, .offset = {0, 0}};

  vkCmdSetScissor(*pArgs->pCommandBuffer, 0, 1, &scissor);

  vkCmdDraw(*pArgs->pCommandBuffer, 3, 1, 0, 0);
  vkCmdEndRenderPass(*pArgs->pCommandBuffer);

  if (vkEndCommandBuffer(*pArgs->pCommandBuffer) != VK_SUCCESS) {
    fprintf(stderr, "Failed to record command buffer\n");
    return 1;
  }

  return 0;
}
