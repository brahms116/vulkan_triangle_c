#include <instance.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int createVulkanInstance(VkInstance *outInstance) {

  const char *layers[1] = {"VK_LAYER_KHRONOS_validation"};

  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, NULL);

  VkLayerProperties *availableLayers =
      malloc(sizeof(VkLayerProperties) * layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

  int layerFound = 0;
  for (uint32_t i = 0; i < layerCount; i++) {
    if (strcmp(layers[0], availableLayers[i].layerName) == 0) {
      layerFound = 1;
      break;
    }
  }

  if (!layerFound) {
    fprintf(stderr, "Validation layer not found\n");
    return 1;
  }

  VkApplicationInfo appInfo = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "Hello Triangle",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0,
  };

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  VkInstanceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledExtensionCount = glfwExtensionCount,
      .ppEnabledExtensionNames = glfwExtensions,
      .enabledLayerCount = 1,
      .ppEnabledLayerNames = layers,
  };

  VkResult result = vkCreateInstance(&createInfo, NULL, outInstance);

  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create instance with VK_SUCCESS \n");
    return 1;
  };
  return 0;
}
