#pragma once

#include "window.hpp"

namespace aito
{

/// <summary>
/// Struct for storing the support details of a swap chain.
/// </summary>
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

/// <summary>
/// Struct for storing the indices of queue families.
/// </summary>
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	
	[[nodiscard]] constexpr bool is_complete() const
	{ return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class Device
{
public:
	explicit Device(Window &window);
	
	~Device();
	
	///// Not copyable or movable
	
	Device(const Device &) = delete;
	
	Device &operator=(const Device &) = delete;
	
	Device(Device &&) = delete;
	
	Device &operator=(Device &&) = delete;
	
	///// Getters
	
	VkCommandPool getCommandPool() { return commandPool_; }
	VkDevice device() { return device_; }
	VkSurfaceKHR surface() { return surface_; }
	VkQueue graphicsQueue() { return graphicsQueue_; }
	VkQueue presentQueue() { return presentQueue_; }

private:
#ifdef NDEBUG
	static constexpr bool enableValidationLayers = false;
#else
	static constexpr bool enableValidationLayers = true;
#endif
	
	Window &window_;
	VkInstance instance_;
	VkDebugUtilsMessengerEXT debugMessenger_;
	
	VkSurfaceKHR surface_;
	VkPhysicalDeviceProperties physicalDeviceProperties_;
	VkPhysicalDevice physicalDevice_;
	VkDevice device_;
	
	VkCommandPool commandPool_;
	VkQueue graphicsQueue_;
	VkQueue presentQueue_;
	
	///// Creation methods
	
	void create_instance();
	
	void setup_debug_messenger();
	
	void pick_physical_device();
	
	void create_surface();
	
	void create_logical_device();
	
	void create_command_pool();
	
	///// Helper methods
	
	bool check_validation_layer_support();
	
	void has_glfw_required_instance_extensions();
	
	bool is_device_suitable(VkPhysicalDevice device);
	
	uint32_t rate_device_suitability(VkPhysicalDevice device);
	
	QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
	
	SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
	
	bool check_device_extension_support(VkPhysicalDevice device);
	
	inline QueueFamilyIndices find_physical_queue_families()
	{ return find_queue_families(physicalDevice_); }
	
	
	const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
	const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	
	///// Static methods
	
	[[nodiscard]] static std::vector<const char *> get_required_extensions();
};
	
}


