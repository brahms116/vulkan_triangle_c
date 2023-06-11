#include <command.h>
#include <draw.h>
#include <stdio.h>

int createSyncObjects(const VkDevice *pDevice,
                      VkSemaphore *outImageAvailableSemaphore,
                      VkSemaphore *outRenderFinishedSemaphore,
                      VkFence *outInFlightFence) {

  VkSemaphoreCreateInfo semaphoreInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };

  VkFenceCreateInfo fenceInfo = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                 .flags = VK_FENCE_CREATE_SIGNALED_BIT};

  VkResult imageAvailableResult = vkCreateSemaphore(
      *pDevice, &semaphoreInfo, NULL, outImageAvailableSemaphore);
  VkResult renderFinishedResult = vkCreateSemaphore(
      *pDevice, &semaphoreInfo, NULL, outRenderFinishedSemaphore);
  VkResult inFlightResult =
      vkCreateFence(*pDevice, &fenceInfo, NULL, outInFlightFence);

  if (imageAvailableResult != VK_SUCCESS ||
      renderFinishedResult != VK_SUCCESS || inFlightResult != VK_SUCCESS) {
    fprintf(stderr, "Failed to create syn objects with VK_SUCCESS");
    return 1;
  }
  return 0;
}

int drawFrame(DrawFrameArgs *pArgs) {

  vkWaitForFences(*pArgs->pDevice, 1, pArgs->pInFlightFence, VK_TRUE,
                  UINT64_MAX);

  vkResetFences(*pArgs->pDevice, 1, pArgs->pInFlightFence);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(*pArgs->pDevice, *pArgs->pSwapchain, UINT64_MAX,
                        *pArgs->pImageAvailableSemaphore, VK_NULL_HANDLE,
                        &imageIndex);

  vkResetCommandBuffer(*pArgs->pCommandBuffer, 0);

  RecordCommandBufferArgs recordArgs = {
      .pDevice = pArgs->pDevice,
      .pCommandBuffer = pArgs->pCommandBuffer,
      .pRenderPass = pArgs->pRenderPass,
      .pSwapchainFramebuffers = pArgs->pFramebuffers,
      .pGraphicsPipeline = pArgs->pGraphicsPipeline,
      .pSwapchainExtent = pArgs->pExtent,
      .swapchainImageIndex = imageIndex,
  };

  if (recordCommandBuffer(&recordArgs)) {
    fprintf(stderr, "Failed to record command buffer");
    return 1;
  }

  VkSemaphore pWaitSemaphores[] = {*pArgs->pImageAvailableSemaphore};
  VkPipelineStageFlags pWaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore pSignalSemaphores[] = {*pArgs->pRenderFinishedSemaphore};

  VkSubmitInfo submitInfo = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = pWaitSemaphores,
    .pWaitDstStageMask = pWaitStages,
    .pSignalSemaphores = pSignalSemaphores,
    .signalSemaphoreCount = 1,
    .commandBufferCount = 1,
    .pCommandBuffers = pArgs->pCommandBuffer,
  };

  if (vkQueueSubmit(*pArgs->pGraphicsQueue, 1, &submitInfo, *pArgs->pInFlightFence)) {
    fprintf(stderr, "Failed to submit draw command buffer");
    return 1;
  }

  VkSwapchainKHR pSwapchains[] = {*pArgs->pSwapchain};

  VkPresentInfoKHR presentInfo = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = pSignalSemaphores,
    .swapchainCount = 1,
    .pSwapchains = pSwapchains,
    .pImageIndices = &imageIndex,
  };

  if (vkQueuePresentKHR(*pArgs->pPresentQueue, &presentInfo)!= VK_SUCCESS) {
    fprintf(stderr, "Failed to present swapchain image");
    return 1;
  }
  return 0;
}
