#pragma once
#include "function/render/mesh.h"
#include "vulkan_vertex_buffer.h"
#include "vulkan_index_buffer.h"

namespace Zafkiel 
{

class VulkanMeshFactory 
{
  public:
    static Scope<Mesh> Create(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager)
    {
        const float *data = reinterpret_cast<const float*>(vertices.data());
        uint32_t size = vertices.size() * sizeof(MeshVertex);
        auto vertexBuffer = VulkanVertexBufferFactory::Create(data, size, device, physicalDevice, commandManager);
        auto indexBuffer = VulkanIndexBufferFactory::Create(indices.data(), indices.size(),  device, physicalDevice, commandManager);
        return CreateScope<Mesh>(std::move(vertexBuffer), std::move(indexBuffer));
    }
};
}
