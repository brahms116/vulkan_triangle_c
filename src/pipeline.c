#include <pipeline.h>
#include <shader.h>
#include <stdio.h>

int createRenderPass(RenderPassArgs *pArgs, VkRenderPass *outRenderPass) {

  VkAttachmentDescription colorAttachment = {
      .format = *pArgs->pSwapchainImageFormat,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentReference colorAttachmentRef = {
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  VkSubpassDescription subpass = {
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachmentRef,
  };

  VkSubpassDependency dependency = {
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT};

  VkRenderPassCreateInfo renderPassInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &colorAttachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
  };

  if (vkCreateRenderPass(*pArgs->pDevice, &renderPassInfo, NULL,
                         outRenderPass) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create render pass\n");
    return 1;
  }

  return 0;
}

int createGraphicsPipeline(GraphicsPipelineArgs *pArgs,
                           VkPipeline *outGraphicsPipeline,
                           VkPipelineLayout *outPipelineLayout) {

  FileContents fragShaderContents = readFile("./shaders/frag.spv");
  FileContents vertShaderContents = readFile("./shaders/vert.spv");

  ShaderModuleArgs vertShaderArgs = {.pDevice = pArgs->pDevice,
                                     .pCode = vertShaderContents.pContent,
                                     .codeSize =
                                         vertShaderContents.contentLength};

  VkShaderModule vertShaderModule;

  if (createShaderModule(&vertShaderArgs, &vertShaderModule)) {
    fprintf(stderr, "Failed to create vertex shader module\n");
    return 1;
  }

  ShaderModuleArgs fragShaderArgs = {.pDevice = pArgs->pDevice,
                                     .pCode = fragShaderContents.pContent,
                                     .codeSize =
                                         fragShaderContents.contentLength};

  VkShaderModule fragShaderModule;

  if (createShaderModule(&fragShaderArgs, &fragShaderModule)) {
    fprintf(stderr, "Failed to create fragment shader module\n");
    return 1;
  }

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertShaderModule,
      .pName = "main"};

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragShaderModule,
      .pName = "main"};

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_SCISSOR,
                                    VK_DYNAMIC_STATE_VIEWPORT};

  VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = 2,
      .pDynamicStates = dynamicStates};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 0,
      .vertexAttributeDescriptionCount = 0};

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      // Experiment with this
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE};

  VkPipelineViewportStateCreateInfo viewportInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .scissorCount = 1};

  VkPipelineRasterizationStateCreateInfo rasterizer = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .rasterizerDiscardEnable = VK_FALSE,
      .lineWidth = 1.0f,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
  };

  // Multisampling and color blend involve combining the fragment shader results
  // with the color that is already in the framebuffer, sort of... not 100% sure
  VkPipelineMultisampleStateCreateInfo multisampling = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .sampleShadingEnable = VK_FALSE,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT};

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_A_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT,
      .blendEnable = VK_FALSE};

  VkPipelineColorBlendStateCreateInfo colorBlending = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment,
  };

  // This is for push constants
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
  };

  if (vkCreatePipelineLayout(*pArgs->pDevice, &pipelineLayoutInfo, NULL,
                             outPipelineLayout) != VK_SUCCESS) {
    fprintf(stderr, "failed to create pipeline layout!\n");
    return 1;
  }

  // Create the graphics pipeline

  VkGraphicsPipelineCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = 2,
      .pStages = shaderStages,
      .pVertexInputState = &vertexInputInfo,
      .pInputAssemblyState = &inputAssemblyInfo,
      .pViewportState = &viewportInfo,
      .pRasterizationState = &rasterizer,
      .pMultisampleState = &multisampling,
      .pColorBlendState = &colorBlending,
      .pDynamicState = &dynamicStateInfo,
      .layout = *outPipelineLayout,
      .subpass = 0,
      .renderPass = *pArgs->pRenderPass,
  };

  if (vkCreateGraphicsPipelines(*pArgs->pDevice, VK_NULL_HANDLE, 1, &createInfo,
                                NULL, outGraphicsPipeline) != VK_SUCCESS) {
    fprintf(stderr, "failed to create graphics pipeline!\n");
    return 1;
  }

  vkDestroyShaderModule(*pArgs->pDevice, fragShaderModule, NULL);
  vkDestroyShaderModule(*pArgs->pDevice, vertShaderModule, NULL);

  return 0;
}
