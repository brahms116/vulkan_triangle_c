#include <cleanup.h>
#include <ext.h>
#include <instance.h>
#include <stdio.h>
#include <glfwSetup.h>

int main() {

  // Window
  GLFWwindow *window = setupGLFWWindow();

  if (glfwVulkanSupported() == GLFW_FALSE) {
    fprintf(stderr, "GLFW failed to find vulkan support\n");
    return 1;
  }

  // Instance
  VkInstance instance;
  if (createVulkanInstance(&instance) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create vulkan instance\n");
    return 1;
  }

  // Surface
  VkSurfaceKHR surface;
  if (setupVulkanSurface(window, &instance, &surface) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create vulkan surface\n");
    return 1;
  }

  // Physical Device
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  const char *physicalDeviceExtensions[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };




  // Cleanup
  CleanupArgs args = {
      .pInstance = &instance,
      .pSurface = &surface,
  };

  cleanup(&args);
  return 0;
}
