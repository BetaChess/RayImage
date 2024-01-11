#include "pch.hpp"

#include "window.hpp"

static void glfw_error_callback(int error, const char *description)
{
	AITO_ERROR("GLFW Error: {}", description);
}

namespace aito
{
Window::Window(size_t w, size_t h, const std::string& name)
	: windowName_(name), width_(w), height_(h)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		AITO_FATAL("GLFW Failed to initialize");
		return;
	}
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window_ = glfwCreateWindow(static_cast<int>(width_), static_cast<int>(height_), name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window_, this);
	glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
	if (!glfwVulkanSupported())
	{
		AITO_FATAL("Vulkan isn't supported");
		return;
	}
	AITO_INFO("GLFW window created");
}

Window::~Window()
= default;

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
	auto AitoWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	
	AitoWindow->frameBufferResized_ = true;
	AitoWindow->width_ = width;
	AitoWindow->height_ = height;
}

void Window::create_window_surface(VkInstance instance, VkSurfaceKHR *surface)
{
	if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface! ");
	}
}
	
} // aito