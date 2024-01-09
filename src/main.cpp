
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "vulkan/window.hpp"

#include "core/logger.hpp"


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
	aito::Logger::init();
	
	aito::Window window(1000, 560, "Test");
	aito::Window window2(1000, 560, "Test2");
	
	std::cin.get();
	
	return 0;
}

