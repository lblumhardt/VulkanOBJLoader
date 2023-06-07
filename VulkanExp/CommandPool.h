#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <vulkan/vulkan.h>

class Device;

class CommandPool {
public:
	CommandPool(const Device& device, const VkCommandPoolCreateFlags& flags);
	~CommandPool();

	inline const VkCommandPool& handle() const { return m_pool; };

private:
	VkCommandPool m_pool;
	VkCommandPoolCreateFlags m_flags;

	const Device& m_device;
};

#endif