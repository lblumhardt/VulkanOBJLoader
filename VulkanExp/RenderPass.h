#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.h>
#include <vector>

class Device;
class SwapChain;

class RenderPass {
public:
	RenderPass(const Device& device, const SwapChain& swapChain);
	~RenderPass();

	void destroyDepthResources();
	void destroyFrameBuffers();
	void CreateDepthResources();
	void CreateFramebuffers();

	inline const VkRenderPass& handle() const { return m_renderPass; }
	inline const VkFramebuffer& frameBuffer(uint32_t index) const { return m_frameBuffers[index]; }
	inline size_t size() const { return m_frameBuffers.size(); }

	void recreate();

private:
	VkRenderPass m_renderPass;

	std::vector<VkFramebuffer> m_frameBuffers;

	VkImage m_depthImage;
	VkDeviceMemory m_depthImageMemory;
	VkImageView m_depthImageView;

	const Device& m_device;
	const SwapChain& m_swapChain;

	void CreateRenderPass();


	VkFormat FindDepthFormat();
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
};

#endif