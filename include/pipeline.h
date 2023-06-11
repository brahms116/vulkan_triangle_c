#ifndef PIPELINE_H
#define PIPELINE_H
#include <ext.h>

typedef struct {
  const VkDevice *pDevice;
  const VkExtent2D *pExtent;
  const VkRenderPass *pRenderPass;
} GraphicsPipelineArgs;

int createGraphicsPipeline(GraphicsPipelineArgs *pArgs,
                           VkPipeline *outGraphicsPipeline,
                           VkPipelineLayout *outPipelineLayout);

typedef struct {
  const VkDevice *pDevice;
  const VkFormat *pSwapchainImageFormat;
} RenderPassArgs;

int createRenderPass(RenderPassArgs *pArgs, VkRenderPass *outRenderPass);

#endif
