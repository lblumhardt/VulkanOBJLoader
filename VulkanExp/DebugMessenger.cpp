#include "DebugMessenger.h"
#include "Instance.h"

#include <iostream>

DebugMessenger::DebugMessenger(const Instance& instance)
	: m_instance(instance)
{
	m_debugMessenger = VK_NULL_HANDLE;

	if (instance.validationLayersEnabled()) {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		// look up address for vkCreateDebugUtilsMessengerEXT
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance.handle(), "vkCreateDebugUtilsMessengerEXT");
		if (func == nullptr) {
			throw std::runtime_error("debug messenger extension unavailable");
		}

		if (func(instance.handle(), &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up a debug messenger");
		}

		/*
		if (CreateDebugUtilsMessengerEXT(instance->getInstance(), &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		}*/
	}
}

DebugMessenger::~DebugMessenger()
{
	if (m_instance.validationLayersEnabled()) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance.handle(), "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(m_instance.handle(), m_debugMessenger, nullptr);
		}
	}
}

void DebugMessenger::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugMessenger::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}