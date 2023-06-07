#ifndef INSTANCE_H
#define INSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

class Instance {
	public:
		Instance(const char* applicationName, const char* engineName, bool validationLayers);
		Instance() = delete;
		~Instance();

		inline const VkInstance& handle() const { return m_instance; }
		inline bool validationLayersEnabled() const { return m_enableValidationLayers; }

		static const std::vector<const char*> ValidationLayers;
		static const std::vector<const char*> DeviceExtensions;

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {

			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}

		//TODO: delete this
		VkInstance getInstance();

	private:
		VkInstance m_instance;
		bool m_enableValidationLayers;

		static bool CheckValidationLayerSupport();
		static void GetRequiredExtensions(std::vector<const char*>& extensions, bool validationLayers);
		void PrintAvailableExtensions();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};

#endif