#include "CommandBuffers.h"

#include <array>

#include "DescriptorSets.h"


CommandBuffers::CommandBuffers(const Device& device, const RenderPass& renderPass, const SwapChain& swapChain, const GraphicsPipeline& graphicsPipeline, const CommandPool& commandPool, int maxFramesInFlight) 
	: m_device(device), m_renderPass(renderPass), m_swapChain(swapChain), m_graphicsPipeline(graphicsPipeline), m_commandPool(commandPool) 
{
	m_commandBuffers.resize(maxFramesInFlight);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool.handle();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	if (vkAllocateCommandBuffers(device.logical(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

CommandBuffers::~CommandBuffers() { destroyCommandBuffers(); }

void CommandBuffers::destroyCommandBuffers() {
	vkFreeCommandBuffers(m_device.logical(), m_commandPool.handle(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
}

void CommandBuffers::ResetCommandBuffer(int currentFrame) {
	vkResetCommandBuffer(m_commandBuffers[currentFrame], 0);
}

void CommandBuffers::RecordCommandBuffer(int currentFrame, int imageIndex, const VkBuffer& vertexBuffer, const VkBuffer& indexBuffer, std::vector<uint32_t> indices, DescriptorSets& descriptorSets)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass.handle();
	renderPassInfo.framebuffer = m_renderPass.frameBuffer(imageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapChain.extent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.7f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(m_commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.pipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_swapChain.extent().width;
	viewport.height = (float)m_swapChain.extent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_commandBuffers[currentFrame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChain.extent();
	vkCmdSetScissor(m_commandBuffers[currentFrame], 0, 1, &scissor);

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(m_commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(m_commandBuffers[currentFrame], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	// &descriptorSets.GetDescriptorSet(currentFrame)
	const VkDescriptorSet set = descriptorSets.GetDescriptorSets()[currentFrame];
	vkCmdBindDescriptorSets(m_commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.layout(), 0, 1, &set, 0, nullptr);

	vkCmdDrawIndexed(m_commandBuffers[currentFrame], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(m_commandBuffers[currentFrame]);

	if (vkEndCommandBuffer(m_commandBuffers[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}


VkCommandBuffer CommandBuffers::BeginSingleTimeCommands(const Device& device, CommandPool& commandPool) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool.handle();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device.logical(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void CommandBuffers::EndSingleTimeCommands(VkCommandBuffer commandBuffer, const Device& device, CommandPool& commandPool) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device.graphicsQueue());

	vkFreeCommandBuffers(device.logical(), commandPool.handle(), 1, &commandBuffer);
}