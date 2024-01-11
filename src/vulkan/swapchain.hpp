#pragma once

#include "device.hpp"

namespace aito
{

class Swapchain
{
public:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
	
	Swapchain(Device &deviceRef, VkExtent2D windowExtent);
	
	Swapchain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<Swapchain> previous);
	
	~Swapchain();
	
	// Not copyable or movable
	Swapchain(const Swapchain &) = delete;
	
	Swapchain &operator=(const Swapchain &) = delete;
	
	Swapchain(Swapchain &&) = delete;
	
	Swapchain &operator=(Swapchain &&) = delete;
	
	
	///// Getters
	
	inline VkFormat getSwapChainImageFormat()
	{ return swapChainImageFormat_; }
	
	inline VkExtent2D getSwapChainExtent()
	{ return swapChainExtent_; }
	
	inline size_t imageCount()
	{ return swapChainImages_.size(); }
	
	[[nodiscard]] inline uint32_t width() const { return swapChainExtent_.width; }
	[[nodiscard]] inline uint32_t height() const { return swapChainExtent_.height; }

	inline VkFramebuffer getFrameBuffer(size_t index) { return swapChainFramebuffers_[index]; }
	inline VkRenderPass getRenderPass() { return renderPass_; }
	inline VkImageView getImageView(size_t index) { return swapChainImageViews_[index]; }

	[[nodiscard]] inline float extentAspectRatio() const
	{
		return static_cast<float>(swapChainExtent_.width) / static_cast<float>(swapChainExtent_.height);
	}
	
	VkFormat findDepthFormat();

	VkResult acquireNextImage(uint32_t* imageIndex);
	VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	[[nodiscard]] inline bool compareSwapChainFormats(const Swapchain& swapChain) const {
		return swapChainDepthFormat_ == swapChain.swapChainDepthFormat_ &&
			   swapChainImageFormat_ == swapChain.swapChainImageFormat_;
	}

private:
	Device &device_;
	VkExtent2D windowExtent_;
	
	VkFormat swapChainImageFormat_;
	VkFormat swapChainDepthFormat_;
	VkExtent2D swapChainExtent_;
	
	std::vector<VkFramebuffer> swapChainFramebuffers_;
	VkRenderPass renderPass_;
	
	std::vector<VkImage> depthImages_;
	std::vector<VkDeviceMemory> depthImageMemories_;
	std::vector<VkImageView> depthImageViews_;
	std::vector<VkImage> swapChainImages_;
	std::vector<VkImageView> swapChainImageViews_;
	
	VkSwapchainKHR swapChain_;
	std::shared_ptr<Swapchain> oldSwapChain_;
	
	std::vector<VkSemaphore> imageAvailableSemaphores_;
	std::vector<VkSemaphore> renderFinishedSemaphores_;
	std::vector<VkFence> inFlightFences_;
	std::vector<VkFence> imagesInFlight_;
	size_t currentFrame_ = 0;
	
	
	void init();
	
	void createSwapChain();
	
	void createImageViews();
	
	void createRenderPass();
	
	void createDepthResources();
	
	void createFramebuffers();
	
	void createSyncObjects();
	
	///// Helper methods
	/// Non-static
	
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
	
	/// Static
	
	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	
	static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	
};
	
}