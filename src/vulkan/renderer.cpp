#include "pch.hpp"

#include "renderer.hpp"

aito::Renderer::Renderer(aito::Window &window, aito::Device &device)
	: window_(window), device_(device)
{
	recreateSwapchain();
	createCommandBuffers();
}

aito::Renderer::~Renderer()
{
	freeCommandBuffers();
}

void aito::Renderer::recreateSwapchain()
{
	auto extent = window_.get_extent();
	
	// This pauses the application if it is minimised.
	while (extent.height == 0 || extent.width == 0)
	{
		extent = window_.get_extent();
		glfwWaitEvents();
	}
	
	// Wait for the swapchain to be unused.
	vkDeviceWaitIdle(device_.device());
	
	if (swapchain_ == nullptr)
	{
		swapchain_ = std::make_unique<Swapchain>(device_, extent);
	}
	else
	{
		std::shared_ptr<Swapchain> oldSwapChain = std::move(swapchain_);
		swapchain_ = std::make_unique<Swapchain>(device_, extent, oldSwapChain);
		
		if (!oldSwapChain->compareSwapChainFormats(*swapchain_.get()))
		{
			AITO_ERROR("Swap chain image (or depth) format has changed");
			throw std::runtime_error("Swap chain image (or depth) format has changed");
		}
	}
}

void aito::Renderer::createCommandBuffers()
{
	commandBuffers_.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);
	
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = device_.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());
	
	if (vkAllocateCommandBuffers(device_.device(), &allocInfo, commandBuffers_.data()) != VK_SUCCESS)
	{
		AITO_FATAL("Failed to allocate command buffers");
		throw std::runtime_error("Failed to allocate command buffers");
	}
}

void aito::Renderer::freeCommandBuffers()
{
	vkFreeCommandBuffers(device_.device(), device_.getCommandPool(), static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
	commandBuffers_.clear();
}

VkCommandBuffer aito::Renderer::beginFrame()
{
	assert(!isFrameStarted_ && "Cannot call beginFrame while already in progress");

	VkResult result = swapchain_->acquireNextImage(&currentImageIndex_);
	
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return nullptr;
	}
	
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		AITO_ERROR("Failed to acquire swap-chain image");
		throw std::runtime_error("Failed to acquire swap-chain image");
	}
	
	isFrameStarted_ = true;
	
	// Get the current command buffer
	auto commandBuffer = getCurrentCommandBuffer();
	
	// "begin" the command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer");
	}
	
	return commandBuffer;
}

void aito::Renderer::endFrame()
{
	assert(isFrameStarted_ && "Can't end frame while frame is not in process");
	auto commandBuffer = getCurrentCommandBuffer();
	
	// "End" the command buffer
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		AITO_ERROR("Failed to end recording of command buffer");
		throw std::runtime_error("Failed to end recording command buffer");
	}
	
	auto result = swapchain_->submitCommandBuffers(&commandBuffer, &currentImageIndex_);
	
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_.wasWindowResized())
	{
		window_.resetWindowResizedFlag();
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		AITO_ERROR("Failed to present swap-chain image");
		throw std::runtime_error("Failed to present swap-chain image");
	}
	
	isFrameStarted_ = false;
	currentFrameIndex_ = (currentFrameIndex_ + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
}

void aito::Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted_ && "Can't begin swap chain render pass while frame is not in progress");
	assert(commandBuffer == getCurrentCommandBuffer() && "Can't start a render pass with a command buffer from a different frame");
	
	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = swapchain_->getRenderPass();
	renderPassBeginInfo.framebuffer = swapchain_->getFrameBuffer(currentImageIndex_);
	
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = swapchain_->getSwapChainExtent();
	
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();
	
	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain_->getSwapChainExtent().width);
	viewport.height = static_cast<float>(swapchain_->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	
	VkRect2D scissor;
	scissor.offset = { 0,0 };
	scissor.extent = swapchain_->getSwapChainExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void aito::Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted_ && "Can't end swap chain render pass while frame is not in progress");
	assert(commandBuffer == getCurrentCommandBuffer() && "Can't end a render pass with a command buffer from a different frame");
	vkCmdEndRenderPass(commandBuffer);
}
