#ifndef CLEANUP_H
#define CLEANUP_H
#include <ext.h>


typedef struct {
  VkImageView *pSwapchainImageViews;
  int swapchainImageViewsCount;
  VkSwapchainKHR *pSwapchain;
  VkInstance *pInstance;
  VkSurfaceKHR *pSurface;
  VkDevice *pDevice;
} CleanupArgs;

void cleanup(CleanupArgs* args);

#endif // CLEANUP_H
