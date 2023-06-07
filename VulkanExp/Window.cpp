#include "Window.h"
#include "Instance.h"

#include <iostream>

Window::Window(const uint32_t widthP, const uint32_t heightP, const Instance& instance)
	:m_instance(instance)
{
	width = widthP;
	height = heightP;
	m_framebufferResized = true;
	glfwWindow = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, FramebufferResizeCallback);

	if (glfwCreateWindowSurface(instance.handle(), glfwWindow, nullptr, &m_surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

Window::~Window()
{
	vkDestroySurfaceKHR(m_instance.handle(), m_surface, nullptr);
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

void Window::mainLoop()
{
	while (!glfwWindowShouldClose(glfwWindow)) {
		glfwPollEvents();
		m_drawFrameHandle(m_framebufferResized);
	}
}

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto w = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	w->m_framebufferResized = true;
	std::cout << "ooo i called the Framebuffer Resize Callback!!! \n";
}

void Window::GetRequiredExtensions(std::vector<const char*>& ret) {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	ret.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

//TODO: remove this DX 
GLFWwindow * Window::getWindow()
{
	return glfwWindow;
}

VkSurfaceKHR Window::getSurface()
{
	return m_surface;
}
