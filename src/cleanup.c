#include <cleanup.h>
#include <global.h>

void cleanup(CleanupArgs *pArgs) {
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(*pArgs->pDevice, pArgs->pRenderFinishedSemaphores[i], NULL);
    vkDestroySemaphore(*pArgs->pDevice, pArgs->pImageAvailableSemaphores[i], NULL);
    vkDestroyFence(*pArgs->pDevice, pArgs->pInFlightFences[i], NULL);
  }
  vkDestroyCommandPool(*pArgs->pDevice, *pArgs->pCommandPool, NULL);
  for (int i = 0; i < pArgs->swapchainImageViewsCount; i++) {
    vkDestroyFramebuffer(*pArgs->pDevice, pArgs->pFramebuffers[i], NULL);
  }
  vkDestroyRenderPass(*pArgs->pDevice, *pArgs->pRenderPass, NULL);
  vkDestroyPipelineLayout(*pArgs->pDevice, *pArgs->pPipelineLayout, NULL);
  for (int i = 0; i < pArgs->swapchainImageViewsCount; i++) {
    vkDestroyImageView(*pArgs->pDevice, pArgs->pSwapchainImageViews[i], NULL);
  }
  vkDestroySwapchainKHR(*pArgs->pDevice, *pArgs->pSwapchain, NULL);
  vkDestroyDevice(*pArgs->pDevice, NULL);
  vkDestroySurfaceKHR(*pArgs->pInstance, *pArgs->pSurface, NULL);
  vkDestroyInstance(*pArgs->pInstance, NULL);
  glfwDestroyWindow(pArgs->pWindow);
  glfwTerminate();
}
