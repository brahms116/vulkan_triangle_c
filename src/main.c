#include <cleanup.h>
#include <command.h>
#include <device.h>
#include <draw.h>
#include <ext.h>
#include <framebuffer.h>
#include <glfwSetup.h>
#include <instance.h>
#include <physicalDevice.h>
#include <pipeline.h>
#include <stdio.h>
#include <stdlib.h>
#include <swapchain.h>

int main() {

  // Window
  GLFWwindow *window = setupGLFWWindow();

  if (glfwVulkanSupported() == GLFW_FALSE) {
    fprintf(stderr, "GLFW failed to find vulkan support\n");
    return 1;
  }

  // Instance
  VkInstance instance;
  if (createVulkanInstance(&instance)) {
    fprintf(stderr, "Failed to create vulkan instance\n");
    return 1;
  }

  // Surface
  VkSurfaceKHR surface;
  if (setupVulkanSurface(window, &instance, &surface)) {
    fprintf(stderr, "Failed to create vulkan surface\n");
    return 1;
  }

  // Physical Device
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  const char *ppPhysicalDeviceExtensions[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  PhysicalDeviceArgs physicalDeviceArgs = {
      .pInstance = &instance,
      .pSurface = &surface,
      .ppDeviceExtensions = ppPhysicalDeviceExtensions,
      .deviceExtensionCount = 1,
  };

  if (pickPhysicalDevice(&physicalDeviceArgs, &physicalDevice)) {
    fprintf(stderr, "Failed to select physical device\n");
    return 1;
  }

  // Device and queues
  VkDevice device;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  DeviceArgs deviceArgs = {
      .pPhysicalDevice = &physicalDevice,
      .pSurface = &surface,
      .ppPhysicalDeviceExtensions = ppPhysicalDeviceExtensions,
      .physicalDeviceExtensionCount = 1,
  };

  if (createDevice(&deviceArgs, &device, &graphicsQueue, &presentQueue)) {
    fprintf(stderr, "Failed to create logical device\n");
    return 1;
  }

  VkFormat swapchainFormat;
  VkExtent2D swapchainExtent;
  VkSwapchainKHR swapchain;

  SwapchainArgs swapchainArgs = {
      .pPhysicalDevice = &physicalDevice,
      .pDevice = &device,
      .pSurface = &surface,
  };

  if (createSwapchain(&swapchainArgs, &swapchain, &swapchainExtent,
                      &swapchainFormat)) {
    fprintf(stderr, "Failed to create swapchain\n");
    return 1;
  }

  // Retreive the number of images in the swapchain to malloc the images and
  // image views
  uint32_t swapchainImageCount;
  vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL);

  VkImage *pSwapchainImages = malloc(sizeof(VkImage) * swapchainImageCount);
  vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount,
                          pSwapchainImages);

  VkImageView *pSwapchainImageViews =
      malloc(sizeof(VkImageView) * swapchainImageCount);

  ImageViewArgs imageViewArgs = {
      .pDevice = &device,
      .pSwapchainImages = pSwapchainImages,
      .swapchainImageViewsCount = swapchainImageCount,
      .pImageFormat = &swapchainFormat,
  };

  if (createSwapchainImageViews(&imageViewArgs, pSwapchainImageViews)) {
    fprintf(stderr, "Failed to create swapchain image views\n");
    return 1;
  }

  // Create the render pass
  VkRenderPass renderPass;

  RenderPassArgs renderPassArgs = {
      .pDevice = &device,
      .pSwapchainImageFormat = &swapchainFormat,
  };

  if (createRenderPass(&renderPassArgs, &renderPass)) {
    fprintf(stderr, "Failed to create render pass\n");
    return 1;
  }

  // Create the graphics pipeline
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  GraphicsPipelineArgs pipelineArgs = {
      .pDevice = &device,
      .pExtent = &swapchainExtent,
      .pRenderPass = &renderPass,
  };

  if (createGraphicsPipeline(&pipelineArgs, &graphicsPipeline,
                             &pipelineLayout)) {
    fprintf(stderr, "Failed to create graphics pipeline\n");
    return 1;
  }

  VkFramebuffer *pSwapchainFramebuffers =
      malloc(sizeof(VkFramebuffer) * swapchainImageCount);

  FramebufferArgs framebufferArgs = {
      .pDevice = &device,
      .pRenderPass = &renderPass,
      .pImageViews = pSwapchainImageViews,
      .imageViewsCount = swapchainImageCount,
      .pExtent = &swapchainExtent,
  };

  if (createFramebuffers(&framebufferArgs, pSwapchainFramebuffers)) {
    fprintf(stderr, "Failed to create framebuffers\n");
    return 1;
  }

  VkCommandPool commandPool;

  CommandPoolArgs commandPoolArgs = {
      .pDevice = &device,
      .pPhysicalDevice = &physicalDevice,
      .pSurface = &surface,
  };

  if (createCommandPool(&commandPoolArgs, &commandPool)) {
    fprintf(stderr, "Failed to create command pool\n");
    return 1;
  }

  VkCommandBuffer cmdBuffer;

  CommandBufferArgs commandBufferArgs = {
      .pDevice = &device,
      .pCommandPool = &commandPool,
  };

  if (createCommandBuffer(&commandBufferArgs, &cmdBuffer)) {
    fprintf(stderr, "Failed to create command buffer\n");
    return 1;
  }

  VkSemaphore imageAvailableSemaphore;
  VkSemaphore renderFinishedSemaphore;
  VkFence inFlightFence;

  if (createSyncObjects(&device, &imageAvailableSemaphore,
                        &renderFinishedSemaphore, &inFlightFence)) {
    fprintf(stderr, "Failed to create sync objects\n");
    return 1;
  }


  DrawFrameArgs frameArgs = {
    .pDevice = &device,
    .pSwapchain = &swapchain,
    .pExtent = &swapchainExtent,
    .pRenderPass = &renderPass,
    .pFramebuffers = pSwapchainFramebuffers,
    .pGraphicsPipeline = &graphicsPipeline,
    .pGraphicsQueue = &graphicsQueue,
    .pPresentQueue = &presentQueue,
    .pCommandBuffer = &cmdBuffer,
    .pImageAvailableSemaphore = &imageAvailableSemaphore,
    .pRenderFinishedSemaphore = &renderFinishedSemaphore,
    .pInFlightFence = &inFlightFence,
  };


  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    drawFrame(&frameArgs);
  }

  vkDeviceWaitIdle(device);

  // Cleanup
  CleanupArgs args = {.pRenderPass = &renderPass,
                      .pFramebuffers = pSwapchainFramebuffers,
                      .pCommandPool = &commandPool,
                      .pGraphicsPipeline = &graphicsPipeline,
                      .pSwapchainImageViews = pSwapchainImageViews,
                      .swapchainImageViewsCount = swapchainImageCount,
                      .pSwapchain = &swapchain,
                      .pInstance = &instance,
                      .pSurface = &surface,
                      .pDevice = &device};

  cleanup(&args);
  return 0;
}
