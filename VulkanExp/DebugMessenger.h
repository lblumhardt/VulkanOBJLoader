#ifndef DEBUGMESSENGER_H
#define DEBUGMESSENGER_H

#include <vulkan/vulkan.h>

class Instance;

class DebugMessenger {
	public:
		DebugMessenger(const Instance& instance);
		~DebugMessenger();

		inline const VkDebugUtilsMessengerEXT& handle() const { return m_debugMessenger; }

		static VKAPI_ATTR VkBool32 VKAPI_CALL
			DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData);

		static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	private:
		const Instance& m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
};

#endif