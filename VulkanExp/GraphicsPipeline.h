#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>

class Device;
class DescriptorSets;
class SwapChain;
class RenderPass;
struct ShaderDetails;

class GraphicsPipeline {
public:
	GraphicsPipeline(const Device& device, const SwapChain& swapChain, const RenderPass& renderPass, DescriptorSets& descriptorSets);
	~GraphicsPipeline();

	void recreate();

	inline const VkPipeline& pipeline() const { return m_pipeline; }
	inline const VkPipelineLayout& layout() const { return m_layout; }

	std::vector<char> ReadFile(const std::string& filename);

private:
	VkPipeline m_pipeline;
	VkPipelineLayout m_layout;
	VkPipelineLayout m_oldLayout;

	const Device& m_device;
	const SwapChain& m_swapChain;
	const RenderPass& m_renderPass;
	DescriptorSets& m_descriptorSets;

	void createPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif