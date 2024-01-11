#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"

namespace aito
{

class Renderer
{
public:
	Renderer(Window& window, Device& device);
	~Renderer();
	
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	
	[[nodiscard]] inline VkRenderPass getSwapChainRenderPass() const { return swapchain_->getRenderPass(); };
	[[nodiscard]] inline bool isFrameInProgress() const { return isFrameStarted_; };
	[[nodiscard]] inline float getAspectRatio() const { return swapchain_->extentAspectRatio(); };

	[[nodiscard]] inline VkCommandBuffer getCurrentCommandBuffer() const
	{
		assert(isFrameStarted_ && "Tried to retrieve command buffer before a frame draw was initialised");
		return commandBuffers_[currentFrameIndex_];
	};

	[[nodiscard]] inline int getFrameIndex() const
	{
		assert(isFrameStarted_ && "Tried to retrieve frame index before a frame dra w was initialised");
		return currentFrameIndex_;
	};

	VkCommandBuffer beginFrame();
	void endFrame();
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
	Window& window_;
	Device& device_;
	std::unique_ptr<Swapchain> swapchain_;
	std::vector<VkCommandBuffer> commandBuffers_;

	uint32_t currentImageIndex_ = 0;
	int currentFrameIndex_ = 0;
	bool isFrameStarted_ = false;

	void createCommandBuffers();
	void freeCommandBuffers();
	void recreateSwapchain();
};

}