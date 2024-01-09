#include "pch.hpp"

#include "window.hpp"

namespace aito
{
Window::Window(size_t w, size_t h, std::string name)
{
//	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		AITO_FATAL("GLFW Failed to initialize");
	}
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", nullptr, nullptr);
	if (!glfwVulkanSupported())
	{
		AITO_FATAL("Vulkan isn't supported");
	}
	
	AITO_INFO("GLFW window created");
}

Window::~Window()
{
	
}
} // aito