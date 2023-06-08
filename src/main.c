#include <cleanup.h>
#include <ext.h>
#include <instance.h>
#include <stdio.h>
#include <glfwSetup.h>
#include <physicalDevice.h>
#include <device.h>

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

  // Cleanup
  CleanupArgs args = {
      .pInstance = &instance,
      .pSurface = &surface,
      .pDevice = &device
  };

  cleanup(&args);
  return 0;
}
