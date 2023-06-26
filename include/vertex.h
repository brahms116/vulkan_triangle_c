#ifndef VERTEX_H
#define VERTEX_H
#include <vector.h>
#include <ext.h>

typedef struct {
  Vec2 position;
  Vec3 color;
} Vertex;


VkVertexInputBindingDescription getVertexBindingDescription() {

  VkVertexInputBindingDescription bindingDescription = {
    .binding = 0,
    // an entire Vertex struct is a single entry
    .stride = sizeof(Vertex),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
  };

  return bindingDescription;
}

VkVertexInputAttributeDescription getVertextPositionAttributeDescription() {
  VkVertexInputAttributeDescription attributeDescription = {
    .location = 0,
    .binding = 0,
    .format = VK_FORMAT_R32G32_SFLOAT,
    .offset = offsetof(Vertex, position)
  };

  return attributeDescription;
}

VkVertexInputAttributeDescription getVertexColorAttributeDescription() {
  VkVertexInputAttributeDescription attributeDescription = {
    .location = 1,
    .binding = 0,
    .format = VK_FORMAT_R32G32B32_SFLOAT,
    .offset = offsetof(Vertex, color)
  };

  return attributeDescription;
}


typedef struct {} VertexBufferArgs;

#endif // VERTEX_H
