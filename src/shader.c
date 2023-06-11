#include <shader.h>
#include <stdio.h>
#include <stdlib.h>


FileContents readFile(const char *filePath) {
  FILE *ptr;
  fopen_s(&ptr, filePath, "rb");
  fseek(ptr, 0, SEEK_END);
  long int size = ftell(ptr);
  char *buffer = malloc(size * sizeof(*buffer));
  fseek(ptr, 0, SEEK_SET);
  fread(buffer, sizeof(*buffer), size, ptr);
  fclose(ptr);
  FileContents fileContents = {.contentLength = size, .pContent = buffer};
  return fileContents;
}

int createShaderModule(ShaderModuleArgs *pArgs, VkShaderModule *outShaderModule){
  VkShaderModuleCreateInfo createInfo = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = pArgs->codeSize,
    .pCode = (uint32_t *)pArgs->pCode
  };
  
  if (vkCreateShaderModule(*pArgs->pDevice, &createInfo, NULL, outShaderModule) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create shader module with VK_SUCCESS\n");
    return 1;
  }
  return 0;
}

