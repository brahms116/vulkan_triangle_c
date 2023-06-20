#ifndef DRAW_H
#define DRAW_H
#include <ext.h>

int createSyncObjects(const VkDevice *pDevice,
                      VkSemaphore *pImageAvailableSemaphore,
                      VkSemaphore *pRenderFinishedSemaphore,
                      VkFence *pInFlightFence);

typedef struct {
  const VkDevice *pDevice;
  const VkRenderPass *pRenderPass;
  const VkPipeline *pGraphicsPipeline;
  const VkQueue *pGraphicsQueue;
  const VkQueue *pPresentQueue;
  const VkCommandBuffer *pCommandBuffer;
  const VkPhysicalDevice *pPhysicalDevice;
  const VkSurfaceKHR *pSurface;
  VkExtent2D *pExtent;
  VkSwapchainKHR *pSwapchain;
  VkFramebuffer *pFramebuffers;
  GLFWwindow *pWindow;
  VkImage *pSwapchainImages;
  uint32_t *pSwapchainImageCount;
  VkFormat *pImageFormat;
  VkImageView *pSwapchainImageViews;
  VkSemaphore *pImageAvailableSemaphores;
  VkSemaphore *pRenderFinishedSemaphores;
  VkFence *pInFlightFences;
  int currentFrame;
} DrawFrameArgs;

int drawFrame(DrawFrameArgs *pArgs);

#endif // DRAW_H
