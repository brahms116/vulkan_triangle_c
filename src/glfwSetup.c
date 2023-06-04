#include <glfwSetup.h>
#include <stdio.h>

GLFWwindow *setupGLFWWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow *window = glfwCreateWindow(800, 600, "Vulkan window", NULL, NULL);
  return window;
}

int setupVulkanSurface(GLFWwindow *pWindow, VkInstance *pInstance,
                       VkSurfaceKHR *outSurface) {

  if (glfwCreateWindowSurface(*pInstance, pWindow, NULL, outSurface) !=
      VK_SUCCESS) {
    fprintf(stderr, "Failed to create window surface with VK_SUCCESS \n");
    return 1;
  }
  return 0;
}
