#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace aito
{

class Window
{
public:
	// Constructor(s)
	Window(size_t w, size_t h, std::string name);
	
	// Delete copy constructor and assignment operator.
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	
	// Destructor
	~Window();

private:
	// Private members
	GLFWwindow* window_;
	std::string windowName_;
	
	size_t width_, height_;
	bool frameBufferResized_ = false;
	
};
	
}
