#include <cleanup.h>

void cleanup(CleanupArgs *pArgs) {

  for (int i = 0; i < pArgs->swapchainImageViewsCount; i++) {
    vkDestroyImageView(*pArgs->pDevice, pArgs->pSwapchainImageViews[i], NULL);
  }
  vkDestroySwapchainKHR(*pArgs->pDevice, *pArgs->pSwapchain, NULL);
  vkDestroyDevice(*pArgs->pDevice, NULL);
  vkDestroySurfaceKHR(*pArgs->pInstance, *pArgs->pSurface, NULL);
  vkDestroyInstance(*pArgs->pInstance, NULL);
}
