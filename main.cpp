#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <optional>
#include <set>
#include <vector>
#include <limits> 
#include <algorithm> 
#include <fstream>
#include <array>
#include <unordered_map>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <chrono>

#include "./VulkanExp/Window.h"
#include "./VulkanExp/Instance.h"
#include "./VulkanExp/DebugMessenger.h"
#include "./VulkanExp/Device.h"
#include "./VulkanExp/SwapChain.h"
#include "./VulkanExp/CommandBuffers.h"
#include "./VulkanExp/GraphicsPipeline.h"
#include "./VulkanExp/FencesAndSemaphores.h"
#include "./VulkanExp/DescriptorSets.h"
#include "./VulkanExp/Model.h"
#include "./VulkanExp/Texture.h"
#include "./VulkanExp/DescriptorSets.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "../models/ariadne.obj";

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
	void run() {
		glfwInit();

		//GLFW was intended for use with OpenGL, so this will tell it not to init an OpenGL contex
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		//Enable window resize
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	VkSurfaceKHR surface;

	Window* window;
	Instance* instance;
	DebugMessenger* debugMessenger;
	Device* device;
	SwapChain* swapChain;
	RenderPass* renderPass;
	GraphicsPipeline* graphicsPipeline;
	CommandPool* commandPool;
	CommandBuffers* commandBuffers;
	FencesAndSemaphores* fencesAndSemaphores;
	Model* currentModel;
	DescriptorSets* descriptorSets;
	Texture* currentTexture;
	//Texture randomTexture;

	uint32_t currentFrame = 0;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	void initVulkan() {
		instance = new Instance("OBJ Viewer", "No Engine", true);
		debugMessenger = new DebugMessenger(*instance);
		initWindow();
		createSurface();
		device = new Device(*instance, *window, Instance::DeviceExtensions);
		swapChain = new SwapChain(*device, *window);
		renderPass = new RenderPass(*device, *swapChain);
		createUniformBuffers();
		commandPool = new CommandPool(*device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		currentTexture = new Texture(*device, *commandPool);
		currentModel = new Model(*device, *commandPool);
		currentModel->LoadModel(MODEL_PATH);
		currentModel->CreateVertexBuffer();
		currentModel->CreateIndexBuffer();
		descriptorSets = new DescriptorSets(*device, MAX_FRAMES_IN_FLIGHT, uniformBuffers, *currentTexture);
		graphicsPipeline = new GraphicsPipeline(*device, *swapChain, *renderPass, *descriptorSets);
		commandBuffers = new CommandBuffers(*device, *renderPass, *swapChain, *graphicsPipeline, *commandPool, MAX_FRAMES_IN_FLIGHT);
		fencesAndSemaphores = new FencesAndSemaphores(*device, swapChain->numImages(), MAX_FRAMES_IN_FLIGHT);		
	}

	void mainLoop() {

		window->setDrawFrameFunc([this](bool& framebufferResized) {
			drawFrame(framebufferResized);
		});

		window->mainLoop();
		vkDeviceWaitIdle(device->logical());
	}

	void cleanup() {
		//destroy all vulkan resources before destroying vulkan instance
		swapChain->~SwapChain();
		graphicsPipeline->~GraphicsPipeline();
		renderPass->~RenderPass();
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(device->logical(), uniformBuffers[i], nullptr);
			vkFreeMemory(device->logical(), uniformBuffersMemory[i], nullptr);
		}
		commandBuffers->~CommandBuffers();
		currentTexture->~Texture();
		currentModel->~Model();
		descriptorSets->~DescriptorSets();

		fencesAndSemaphores->~FencesAndSemaphores();
		device->~Device();
		debugMessenger->~DebugMessenger();

		//destroy vkInstance
		instance->~Instance();

		//Destroy window and terminate GLFW
		window->~Window();
	}

	void initWindow() {
		window = new Window(WIDTH, HEIGHT, *instance);
	}

	void printAvailableExtensions() {
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::cout << "available extensions:\n";

		for (const auto& extension : extensions) {
			std::cout << '\t' << extension.extensionName << '\n';
		}
	}

	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void createSurface() {		
		//TODO: temp while i refactor
		surface = window->getSurface();
	}

	void drawFrame(bool& framebufferResized) {
		vkWaitForFences(device->logical(), 1, &fencesAndSemaphores->inFlightFence(currentFrame), VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device->logical(), swapChain->handle(), UINT64_MAX, fencesAndSemaphores->imageAvailable(currentFrame), VK_NULL_HANDLE, &imageIndex);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain(framebufferResized);
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		updateUniformBuffer(currentFrame);

		vkResetFences(device->logical(), 1, &fencesAndSemaphores->inFlightFence(currentFrame));

		commandBuffers->ResetCommandBuffer(currentFrame);
		commandBuffers->RecordCommandBuffer(currentFrame, imageIndex, currentModel->GetVertextBuffer(), currentModel->GetIndexBuffer(), currentModel->GetIndices(), *descriptorSets);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers->command(currentFrame);

		VkSemaphore waitSemaphores[] = { fencesAndSemaphores->imageAvailable(currentFrame) };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		VkSemaphore signalSemaphores[] = { fencesAndSemaphores->renderFinished(currentFrame) };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(device->graphicsQueue(), 1, &submitInfo, fencesAndSemaphores->inFlightFence(currentFrame)) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain->handle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(device->presentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
			recreateSwapChain(framebufferResized);
			framebufferResized = false;
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	}

	void recreateSwapChain(bool& framebufferResized) {
		framebufferResized = true;

		int awidth = 800;
		int aheight = 400;
		window->framebufferSize(awidth, aheight);
		while (awidth == 0 || aheight == 0) {
			window->framebufferSize(awidth, aheight);
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(device->logical());

		//Destroy depth resources
		renderPass->destroyDepthResources();
		//Destroy framebuffers
		renderPass->destroyFrameBuffers();
		//Destroy image views
		swapChain->destroyImageViews();
		//Destroy swap chain
		swapChain->destroySwapChain();
		
		//Create swapchain
		swapChain->createSwapChain();
		//Create image views
		swapChain->createImageViews();
		//Create depth resources
		renderPass->CreateDepthResources();
		//Create frame buffers
		renderPass->CreateFramebuffers();

		//graphicsPipeline->recreate();

		//renderPass->recreate();//!!!!!

		/*renderPass->recreate();
		graphicsPipeline->recreate();
		commandBuffers->recreate();
		renderPass->cleanupOld();*/
	}

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(device->physical(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}


	void createUniformBuffers() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			Model::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i], *device);

			vkMapMemory(device->logical(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void updateUniformBuffer(uint32_t currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), swapChain->extent().width / (float)swapChain->extent().height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
};

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}