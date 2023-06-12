#include <command.h>
#include <draw.h>
#include <global.h>
#include <stdio.h>

int createSyncObjects(const VkDevice *pDevice,
                      VkSemaphore *outImageAvailableSemaphores,
                      VkSemaphore *outRenderFinishedSemaphores,
                      VkFence *outInFlightFences) {

  VkSemaphoreCreateInfo semaphoreInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };

  VkFenceCreateInfo fenceInfo = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                 .flags = VK_FENCE_CREATE_SIGNALED_BIT};

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkResult imageAvailableResult = vkCreateSemaphore(
        *pDevice, &semaphoreInfo, NULL, &outImageAvailableSemaphores[i]);
    VkResult renderFinishedResult = vkCreateSemaphore(
        *pDevice, &semaphoreInfo, NULL, &outRenderFinishedSemaphores[i]);
    VkResult inFlightResult =
        vkCreateFence(*pDevice, &fenceInfo, NULL, &outInFlightFences[i]);

    if (imageAvailableResult != VK_SUCCESS ||
        renderFinishedResult != VK_SUCCESS || inFlightResult != VK_SUCCESS) {
      fprintf(stderr, "Failed to create syn objects with VK_SUCCESS");
      return 1;
    }
  }
  return 0;
}

int drawFrame(DrawFrameArgs *pArgs) {

  vkWaitForFences(*pArgs->pDevice, 1,
                  &pArgs->pInFlightFences[pArgs->currentFrame], VK_TRUE,
                  UINT64_MAX);

  vkResetFences(*pArgs->pDevice, 1,
                &pArgs->pInFlightFences[pArgs->currentFrame]);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(*pArgs->pDevice, *pArgs->pSwapchain, UINT64_MAX,
                        pArgs->pImageAvailableSemaphores[pArgs->currentFrame],
                        VK_NULL_HANDLE, &imageIndex);

  vkResetCommandBuffer(pArgs->pCommandBuffer[pArgs->currentFrame], 0);

  RecordCommandBufferArgs recordArgs = {
      .pDevice = pArgs->pDevice,
      .pCommandBuffer = &pArgs->pCommandBuffer[pArgs->currentFrame],
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

  VkSemaphore pWaitSemaphores[] = {
      pArgs->pImageAvailableSemaphores[pArgs->currentFrame]};
  VkPipelineStageFlags pWaitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore pSignalSemaphores[] = {
      pArgs->pRenderFinishedSemaphores[pArgs->currentFrame]};

  VkSubmitInfo submitInfo = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = pWaitSemaphores,
      .pWaitDstStageMask = pWaitStages,
      .pSignalSemaphores = pSignalSemaphores,
      .signalSemaphoreCount = 1,
      .commandBufferCount = 1,
      .pCommandBuffers = &pArgs->pCommandBuffer[pArgs->currentFrame],
  };

  if (vkQueueSubmit(*pArgs->pGraphicsQueue, 1, &submitInfo,
                    pArgs->pInFlightFences[pArgs->currentFrame])) {
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

  if (vkQueuePresentKHR(*pArgs->pPresentQueue, &presentInfo) != VK_SUCCESS) {
    fprintf(stderr, "Failed to present swapchain image");
    return 1;
  }
  return 0;
}
