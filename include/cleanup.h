#ifndef CLEANUP_H
#define CLEANUP_H
#include <ext.h>


typedef struct {
  VkCommandPool *pCommandPool;
  VkFramebuffer *pFramebuffers;
  VkImageView *pSwapchainImageViews;
  int swapchainImageViewsCount;
  VkSwapchainKHR *pSwapchain;
  VkInstance *pInstance;
  VkSurfaceKHR *pSurface;
  VkDevice *pDevice;
  GLFWwindow *pWindow;
  VkRenderPass *pRenderPass;
  VkPipelineLayout *pPipelineLayout;
  VkPipeline *pGraphicsPipeline;
  VkSemaphore *pImageAvailableSemaphores;
  VkSemaphore *pRenderFinishedSemaphores;
  VkFence *pInFlightFences;
} CleanupArgs;

void cleanup(CleanupArgs* args);

#endif // CLEANUP_H
