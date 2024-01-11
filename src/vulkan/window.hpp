#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <string>

namespace aito
{

class Window
{
public:
	///// Constructor(s)
	
	Window(size_t w, size_t h, const std::string& name);
	
	// Delete copy constructor and assignment operator.
	Window(const Window &) = delete;
	
	Window &operator=(const Window &) = delete;
	
	// Destructor
	~Window();
	
	///// Public methods
	void create_window_surface(VkInstance instance, VkSurfaceKHR *surface);
	[[nodiscard]] inline VkExtent2D get_extent() const { return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_) }; }
	inline bool shouldClose() { return glfwWindowShouldClose(window_); };
	[[nodiscard]] inline bool wasWindowResized() const { return frameBufferResized_; };
	inline void resetWindowResizedFlag() { frameBufferResized_ = false; };
	
	
	[[nodiscard]] inline GLFWwindow *get_glfw_window() const
	{ return window_; };

private:
	// Private members
	GLFWwindow *window_;
	std::string windowName_;
	
	size_t width_, height_;
	bool frameBufferResized_ = false;
	
	// Static
	static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
};
	
}
