#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <ext.h>

typedef struct {
  const VkDevice *pDevice;
  const VkRenderPass *pRenderPass;
  const VkImageView *pImageViews;
  uint32_t imageViewsCount;
  const VkExtent2D *pExtent;
} FramebufferArgs;

int createFramebuffers(FramebufferArgs *pArgs, VkFramebuffer *outFramebuffers);

typedef struct {
  const VkDevice *pDevice;
  const VkRenderPass *pRenderPass;
  const VkExtent2D *pSwapchainExtent;
  const VkSwapchainKHR *pSwapchain;
  const VkFormat *pSwapchainFormat;
} FramebufferAndImagesArgs;

typedef struct {
  int success;
  VkFramebuffer *pFramebuffers;
  VkImage *pImages;
  VkImageView *pImageViews;
  uint32_t swapchainImageCount;
} FramebufferAndImages;

FramebufferAndImages createFramebufferAndImages(FramebufferAndImagesArgs *pArgs);

typedef struct {
  const VkDevice *pDevice;
  VkFramebuffer *pFramebuffers;
  VkImage *pImages;
  VkImageView *pImageViews;
  int swapchainImageCount;
} CleanupFramebufferAndImagesArgs;

void cleanupFramebufferAndImages(CleanupFramebufferAndImagesArgs *pArgs);

#endif // FRAMEBUFFER_H
