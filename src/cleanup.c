#include <cleanup.h>

void cleanup(CleanupArgs *pArgs) {
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
