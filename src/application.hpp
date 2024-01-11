#pragma once

#include "vulkan/window.hpp"
#include "vulkan/device.hpp"
#include "vulkan/renderer.hpp"

namespace aito
{

class Application
{
public:
	static constexpr size_t WIDTH = 800;
	static constexpr size_t HEIGHT = 600;
	
	Application();
	~Application();
	
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	
//	void run();

private:
	Window window_{ WIDTH, HEIGHT, "Aito" };
	Device device_{ window_ };
	Renderer renderer_{ window_, device_ };
	
//	std::unique_ptr<DescriptorPool> globalPool_{ DescriptorPool::Builder(device_)
//														 .setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
//														 .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT)
//														 .build() };
};

}