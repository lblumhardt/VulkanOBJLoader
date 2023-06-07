#ifndef QUEUEFAMILY_H
#define QUEUEFAMILY_H

#include <vulkan/vulkan.h>
#include <optional>

struct QueueFamilyIndices {
	// Drawing command support
	std::optional<uint32_t> graphicsFamily;
	// Support for drawing to surface
	std::optional<uint32_t> presentFamily;

	inline bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class QueueFamily {
public:
	QueueFamily() = delete;
	~QueueFamily() = delete;

	static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device,
		const VkSurfaceKHR& surface);
};

#endif QUEUEFAMILY_H