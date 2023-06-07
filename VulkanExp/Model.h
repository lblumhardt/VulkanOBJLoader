#ifndef MODEL_H
#define MODEL_H

#include <vulkan/vulkan.h>
#include <vector>

#include "Vertex.h"

class Device;
class CommandBuffers;
class CommandPool;

class Model {

	public:
		Model(const Device & device, CommandPool& commandPool);
		~Model();

		void LoadModel(std::string modelPath);
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		inline VkBuffer GetVertextBuffer() { return m_vertexBuffer; }
		inline VkBuffer GetIndexBuffer() { return m_indexBuffer; }
		inline std::vector<uint32_t> GetIndices() { return m_indices; }
		inline std::vector<Vertex> GetVertices() { return m_vertices; }

		static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const Device& device);
		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const Device& device);


	private:
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;

		const Device& m_device;
		CommandPool& m_commandPool;

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer destBuffer, VkDeviceSize size);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

};

#endif
