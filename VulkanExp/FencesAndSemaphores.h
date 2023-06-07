#ifndef FENCESANDSEMAPHORES_H
#define FENCESANDSEMAPHORES_H

#include <vulkan/vulkan.h>
#include <vector>

class Device;

class FencesAndSemaphores {
	public:
		FencesAndSemaphores(const Device& device, uint32_t numImages, uint32_t maxFramesInFlight);
		~FencesAndSemaphores();

		inline VkSemaphore& imageAvailable(uint32_t index) { return m_imageAvailable[index]; }
		inline VkSemaphore& renderFinished(uint32_t index) { return m_renderFinished[index]; }
		inline VkFence& inFlightFence(uint32_t index) { return m_inFlightFences[index]; }

	private:
		const Device& m_device;

		uint32_t m_numImages, m_maxFramesInFlight;

		std::vector<VkSemaphore> m_imageAvailable;
		std::vector<VkSemaphore> m_renderFinished;
		std::vector<VkFence> m_inFlightFences;
};


#endif