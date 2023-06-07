#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#include <vulkan/vulkan.h>
#include <functional>

#include "CommandPool.h"
#include "Device.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "DescriptorSets.h"

class CommandBuffers {
	public:
		CommandBuffers(const Device& device, const RenderPass& renderpass, const SwapChain& swapChain, const GraphicsPipeline& graphicsPipeline, const CommandPool& commandPool, int maxFramesInFlight);
		~CommandBuffers();

		inline VkCommandBuffer& command(uint32_t index) { return m_commandBuffers[index]; }
		inline const VkCommandBuffer& command(uint32_t index) const { return m_commandBuffers[index]; }

		void ResetCommandBuffer(int currentFrame);
		void RecordCommandBuffer(int currentFrame, int imageIndex, const VkBuffer& vertexBuffer, const VkBuffer& indexBuffer, std::vector<uint32_t> indices, DescriptorSets& descriptorSets);

		static VkCommandBuffer BeginSingleTimeCommands(const Device& device, CommandPool& commandPool);
		static void EndSingleTimeCommands(VkCommandBuffer commandBuffer, const Device& device, CommandPool& commandPool);

	protected:
		std::vector<VkCommandBuffer> m_commandBuffers;

		const Device& m_device;
		const RenderPass& m_renderPass;
		const SwapChain& m_swapChain;
		const GraphicsPipeline& m_graphicsPipeline;
		const CommandPool& m_commandPool;

		void createCommandBuffers();
		void destroyCommandBuffers();
};

#endif