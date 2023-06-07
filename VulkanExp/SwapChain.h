#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class Window;

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {
	public:
		explicit SwapChain(const Device& device, const Window& window);
		~SwapChain();

		auto recreate() -> void;

		inline const VkSwapchainKHR& handle() const { return m_swapChain; }
		inline const VkFormat& imageFormat() const { return m_imageFormat; }
		inline const VkExtent2D& extent() const { return m_extent; }
		inline size_t numImages() const { return m_images.size(); }
		inline size_t numImageViews() const { return m_imageViews.size(); }

		inline const SwapChainSupportDetails& supportDetails() const { return m_supportDetails; }
		inline VkImageView imageView(uint32_t index) const { return m_imageViews[index]; }

		static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window);

		void destroyImageViews();
		void destroySwapChain();

		void CleanupSwapChain();
		void createSwapChain();
		void createImageViews();

	private:
		const Device& m_device;
		const Window& m_window;

		SwapChainSupportDetails m_supportDetails;
		VkSwapchainKHR m_swapChain;

		// Swap chain image handles
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;

		VkFormat m_imageFormat;
		VkExtent2D m_extent;

		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
};

#endif