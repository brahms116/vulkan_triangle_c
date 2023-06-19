#include <cleanup.h>
#include <command.h>
#include <device.h>
#include <draw.h>
#include <ext.h>
#include <framebuffer.h>
#include <glfwSetup.h>
#include <global.h>
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

  FramebufferAndImagesArgs framebufferAndImageArgs = {
      .pRenderPass = &renderPass,
      .pDevice = &device,
      .pSwapchainExtent = &swapchainExtent,
      .pSwapchainFormat = &swapchainFormat,
  };

  FramebufferAndImages framebufferAndImages =
      createFramebufferAndImages(&framebufferAndImageArgs);

  if (framebufferAndImages.success == 0) {
    fprintf(stderr, "Failed to create framebuffer and images\n");
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

  VkCommandBuffer pCmdBuffers =
      malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);

  CommandBufferArgs commandBufferArgs = {
      .pDevice = &device,
      .pCommandPool = &commandPool,
  };

  if (createCommandBuffers(&commandBufferArgs, &pCmdBuffers)) {
    fprintf(stderr, "Failed to create command buffer\n");
    return 1;
  }

  VkSemaphore *pImageAvailableSemaphores =
      malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
  VkSemaphore *pRenderFinishedSemaphores =
      malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
  VkFence *pInFlightFences = malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

  if (createSyncObjects(&device, pImageAvailableSemaphores,
                        pRenderFinishedSemaphores, pInFlightFences)) {
    fprintf(stderr, "Failed to create sync objects\n");
    return 1;
  }

  int currentFrame = 0;

  while (!glfwWindowShouldClose(window)) {
    printf("Frame %d\n", currentFrame);
    DrawFrameArgs frameArgs = {
        .pDevice = &device,
        .pSwapchain = &swapchain,
        .pExtent = &swapchainExtent,
        .pRenderPass = &renderPass,
        .pFramebuffers = framebufferAndImages.pFramebuffers,
        .pGraphicsPipeline = &graphicsPipeline,
        .pGraphicsQueue = &graphicsQueue,
        .pPresentQueue = &presentQueue,
        .pCommandBuffer = &pCmdBuffers,
        .pImageAvailableSemaphores = pImageAvailableSemaphores,
        .pRenderFinishedSemaphores = pRenderFinishedSemaphores,
        .pInFlightFences = pInFlightFences,
        .currentFrame = currentFrame,
    };
    glfwPollEvents();
    drawFrame(&frameArgs);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  vkDeviceWaitIdle(device);

  // New cleanup for framebuffer, images, and image views
  CleanupFramebufferAndImagesArgs cleanupFramebufferArgs = {
      .pDevice = &device,
      .pFramebuffers = framebufferAndImages.pFramebuffers,
      .swapchainImageCount = framebufferAndImages.swapchainImageCount,
      .pImageViews = framebufferAndImages.pImageViews,
      .pImages = framebufferAndImages.pImages,
  };
  cleanupFramebufferAndImages(&cleanupFramebufferArgs);

  // Old Cleanup
  CleanupArgs args = {
      .pRenderPass = &renderPass,
      .pCommandPool = &commandPool,
      .pGraphicsPipeline = &graphicsPipeline,
      .swapchainImageViewsCount = framebufferAndImages.swapchainImageCount,
      .pSwapchain = &swapchain,
      .pInstance = &instance,
      .pSurface = &surface,
      .pDevice = &device,
      .pImageAvailableSemaphores = pImageAvailableSemaphores,
      .pRenderFinishedSemaphores = pRenderFinishedSemaphores,
      .pInFlightFences = pInFlightFences,
  };

  cleanup(&args);
  free(pImageAvailableSemaphores);
  free(pRenderFinishedSemaphores);
  free(pInFlightFences);
  return 0;
}
