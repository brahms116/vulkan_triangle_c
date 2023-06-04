#ifndef GLFWSETUP_H
#define GLFWSETUP_H
#include <ext.h>

GLFWwindow *setupGLFWWindow();

int setupVulkanSurface(GLFWwindow *pWindow, VkInstance *pInstance,
                       VkSurfaceKHR *outSurface);

#endif // GLFWSETUP_H
