#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class DescriptorSets {

	public:
		DescriptorSets(const Device& device, uint32_t maxFramesInFlight, std::vector<VkBuffer> uniformBuffers, Texture& texture);
		~DescriptorSets();

		inline const std::vector<VkDescriptorSet> GetDescriptorSets() { return m_descriptorSets; }
		inline const VkDescriptorSetLayout GetLayout() { return m_descriptorSetLayout; }

	private:
		Device m_device;

		VkDescriptorPool m_descriptorPool;
		std::vector<VkDescriptorSet> m_descriptorSets;
		VkDescriptorSetLayout m_descriptorSetLayout;
		uint32_t m_maxFramesInFlight;

		std::vector<VkBuffer> m_uniformBuffers;

};

#endif
