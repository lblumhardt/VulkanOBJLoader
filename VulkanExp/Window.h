#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <functional>

class Instance;

class Window {

	public:

		Window(const uint32_t width, const uint32_t height, const Instance& instance);
		Window() = delete;
		~Window();
		void mainLoop();
		inline void setDrawFrameFunc(const std::function<void(bool&)>& func) { m_drawFrameHandle = func; }
		static void GetRequiredExtensions(std::vector<const char*>& ret);
		inline const VkSurfaceKHR& surface() const { return m_surface; }

		inline const uint32_t getWidth() const { return width; }
		inline const uint32_t getHeight() const { return height; }
		inline void framebufferSize(int awidth, int aheight) const {
			glfwGetFramebufferSize(glfwWindow, &awidth, &aheight);
		}

		//TODO: delete this once we're done adding classes
		GLFWwindow* getWindow();
		VkSurfaceKHR getSurface();

	private:
		GLFWwindow* glfwWindow;
		std::function<void(bool&)> m_drawFrameHandle;

		const Instance& m_instance;

		uint32_t width;
		uint32_t height;
		VkSurfaceKHR m_surface;
		bool m_framebufferResized;
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

};

#endif