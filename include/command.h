#ifndef COMMAND_H
#define COMMAND_H
#include<ext.h>


typedef struct {
  const VkDevice *pDevice;
  const VkPhysicalDevice *pPhysicalDevice;
  const VkSurfaceKHR *pSurface;
} CommandPoolArgs;

int createCommandPool(CommandPoolArgs *pArgs, VkCommandPool* outCommandPool);

typedef struct {
  const VkDevice *pDevice;
  const VkCommandPool *pCommandPool;
} CommandBufferArgs;

int createCommandBuffers(CommandBufferArgs *pArgs, VkCommandBuffer* outCommandBuffer);

typedef struct {
  const VkDevice *pDevice;
  const VkCommandBuffer *pCommandBuffer;
  uint32_t swapchainImageIndex;
  const VkRenderPass *pRenderPass;
  const VkFramebuffer *pSwapchainFramebuffers;
  const VkPipeline *pGraphicsPipeline;
  const VkExtent2D *pSwapchainExtent;
} RecordCommandBufferArgs;

int recordCommandBuffer(RecordCommandBufferArgs *pArgs);

#endif // COMMAND_H
