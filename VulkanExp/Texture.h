#ifndef TEXTURE_H
#define TEXTURE_H

#include <vulkan/vulkan.h>

#include "CommandPool.h"
#include "Device.h"

class Texture {
	public:
		Texture(const Device& device, CommandPool& commandPool);
		~Texture();

		inline const VkImageView imageView() { return m_textureImageView; }
		inline const VkSampler sampler() { return m_textureSampler; }

	private:

		VkImage m_textureImage;
		uint32_t m_mipLevels;
		VkDeviceMemory m_textureImageMemory;
		VkImageView m_textureImageView;
		VkSampler m_textureSampler;

		CommandPool m_commandPool;
		Device m_device;

		void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

};

#endif