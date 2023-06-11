#ifndef DRAW_H
#define DRAW_H
#include <ext.h>

int createSyncObjects(const VkDevice *pDevice,
                      VkSemaphore *pImageAvailableSemaphore,
                      VkSemaphore *pRenderFinishedSemaphore,
                      VkFence *pInFlightFence);

typedef struct {
  const VkDevice *pDevice;
  const VkSwapchainKHR *pSwapchain;
  const VkExtent2D *pExtent;
  const VkRenderPass *pRenderPass;
  const VkFramebuffer *pFramebuffers;
  const VkPipeline *pGraphicsPipeline;
  const VkQueue *pGraphicsQueue;
  const VkQueue *pPresentQueue;
  const VkCommandBuffer *pCommandBuffer;
  VkSemaphore *pImageAvailableSemaphore;
  VkSemaphore *pRenderFinishedSemaphore;
  VkFence *pInFlightFence;
} DrawFrameArgs;

int drawFrame(DrawFrameArgs *pArgs);

#endif // DRAW_H
