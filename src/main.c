#include <cleanup.h>
#include <ext.h>
#include <instance.h>
#include <stdio.h>
#include <glfwSetup.h>
#include <physicalDevice.h>
#include <device.h>
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

  const char *physicalDeviceExtensions[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  PhysicalDeviceArgs physicalDeviceArgs = {
      .pInstance = &instance,
      .pSurface = &surface,
      .ppDeviceExtensions = physicalDeviceExtensions,
      .deviceExtensionCount = 1,
  };

  if(pickPhysicalDevice(&physicalDeviceArgs, &physicalDevice)) {
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
      .ppPhysicalDeviceExtensions = physicalDeviceExtensions,
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

  if (createSwapchain(&swapchainArgs, &swapchain, &swapchainExtent, &swapchainFormat)) {
    fprintf(stderr, "Failed to create swapchain\n");
    return 1;
  }

  // Retreive the number of images in the swapchain to malloc the images and image views
  uint32_t swapchainImageCount;
  vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL);

  VkImage* pSwapchainImages = malloc(sizeof(VkImage) * swapchainImageCount);
  vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, pSwapchainImages);

  VkImageView* pSwapchainImageViews = malloc(sizeof(VkImageView) * swapchainImageCount);

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
  

  // Cleanup
  CleanupArgs args = {
      .pSwapchainImageViews = pSwapchainImageViews,
      .swapchainImageViewsCount = swapchainImageCount,
      .pSwapchain = &swapchain,
      .pInstance = &instance,
      .pSurface = &surface,
      .pDevice = &device
  };

  cleanup(&args);
  return 0;
}
