#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include<ext.h>


typedef struct {
  const VkDevice *pDevice;
  const VkRenderPass *pRenderPass;
  const VkImageView *pImageViews;
  uint32_t imageViewsCount;
  const VkExtent2D *pExtent;
} FramebufferArgs;

int createFramebuffers(FramebufferArgs *pArgs, VkFramebuffer* outFramebuffers);

#endif // FRAMEBUFFER_H
