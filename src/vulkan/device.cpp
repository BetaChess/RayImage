#include "pch.hpp"

#include "device.hpp"

#include <unordered_set>
#include <set>
#include <map>
#include <sstream>

namespace aito
{

///// Local helper functions
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
		void *pUserData)
{
	
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		AITO_ERROR("VK_VALIDATION {}", pCallbackData->pMessage);
	} else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		AITO_WARN("VK_VALIDATION {}", pCallbackData->pMessage);
	} else
	{
		AITO_TRACE("VK_VALIDATION {}", pCallbackData->pMessage);
	}
	
	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance_,
		const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
		const VkAllocationCallbacks *pAllocator,
		VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
			instance_,
			"vkCreateDebugUtilsMessengerEXT");
	
	if (func != nullptr)
	{
		return func(instance_, pCreateInfo, pAllocator, pDebugMessenger);
	} else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(
		VkInstance instance_,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks *pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
			instance_,
			"vkDestroyDebugUtilsMessengerEXT");
	
	if (func != nullptr)
	{
		func(instance_, debugMessenger, pAllocator);
	}
	
}



///// Member functions

Device::Device(Window &window)
		: window_(window)
{
	AITO_INFO("Creating Vulkan Instance");
	create_instance();
	AITO_INFO("Setting up debug messenger");
	setup_debug_messenger();
	AITO_INFO("Creating Vulkan surface");
	create_surface();
	AITO_INFO("Picking physical device");
	pick_physical_device();
	AITO_INFO("Creating logical device");
	create_logical_device();
	AITO_INFO("Creating command pool");
	create_command_pool();
}

Device::~Device()
{
	// Free handles in reverse order
	vkDestroyCommandPool(device_, commandPool_, nullptr);
	vkDestroyDevice(device_, nullptr);
	vkDestroySurfaceKHR(instance_, surface_, nullptr);
	
	if constexpr (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
	}
	vkDestroyInstance(instance_, nullptr);
}

/// <summary>
/// Method for getting the extensions required by the application.
/// </summary>
/// <returns>A vector of all of the required extensions. </returns>
std::vector<const char *> Device::get_required_extensions()
{
	// Get the number of extensions required by glfw.
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	// Put them in the vector of required extensions
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	
	// If validation layers are enabled. Throw in the Debug Utils extension.
	if constexpr (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	
	// Return the vector.
	return extensions;
}

/// <summary>
/// Check if all of the required extensions are supported. Throws a runtime error if they are not.
/// </summary>
void Device::has_glfw_required_instance_extensions()
{
	// Create a variable to store the number of supported extensions
	uint32_t extensionCount = 0;
	
	// Get the number of avaliable extensions. (The middle parameter of the function writes this to the specified memory location)
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	// Change the size of the extensions vetor to match the number of avaliable extensions.
	std::vector<VkExtensionProperties> extensions(extensionCount);
	// Write the extensions to the vector. 
	// (the last parameter is a memory location to the place the extension properties should be written, 
	//		in this case, the data location of the vector)
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	
	// List out the avaliable extensions and put the names in a string data structure.
	std::unordered_set<std::string> available;
	{
		std::stringstream ss;
		ss << "\n available extensions:";
		for (const auto &extension: extensions)
		{
			ss << "\n\t" << extension.extensionName;
			available.insert(extension.extensionName);
		}
		
		AITO_TRACE(ss.str());
	}
	
	{
		std::stringstream ss;
		// List out the required extensions.
		ss << "\n required extensions:";
		// Get the required extensions.
		auto requiredExtensions = get_required_extensions();
		// Iterate through the required extensions.
		for (const auto &required: requiredExtensions)
		{
			ss << "\n\t" << required;
			// If the required extension is not available, throw a runtime error.
			if (available.find(required) == available.end())
			{
				AITO_FATAL("Missing required glfw extension ({})", required);
				throw std::runtime_error("Missing required glfw extension");
			}
		}
		
		AITO_TRACE(ss.str());
	}
}

void Device::create_instance()
{
	AITO_INFO("Checking validation layer support");
	// Check if the validation layers are enabled and supported.
	if (enableValidationLayers && !check_validation_layer_support())
	{
		// If they are enabled but not supported, throw a runtime error.
		throw std::runtime_error("Validation layers requested, but not available! ");
	}
	
	// Create the appinfo struct.
	VkApplicationInfo appInfo{};
	// Specify the struct as a type application info. (a struct used to give information about the instance).
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	// Set the name
	// TODO: Change this to take the actual name
	appInfo.pApplicationName = "Aito";
	// Specify the application version
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	// A custom engine is used.
	appInfo.pEngineName = "Custom Engine (Aito)";
	// Specify the engine version
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	// Specify the vulkan API version.
	appInfo.apiVersion = VK_API_VERSION_1_0;
	
	// Create the instance_ create info
	VkInstanceCreateInfo createInfo{};
	// Specify that this struct is the info to create the instance.
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	// give it the application info created earlier.
	createInfo.pApplicationInfo = &appInfo;
	
	// Get and enable the glfw extensions
	auto extensions = get_required_extensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());        // The number of enabled extensions
	createInfo.ppEnabledExtensionNames = extensions.data();                                // The names of the actual extensions
	
	// create a temporary debugger for creation of the Vulkan instance_
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	// check if validation layers are enabled.
	if constexpr (enableValidationLayers)
	{
		// Specify the enabled validation layers
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());    // The number of validation layers
		createInfo.ppEnabledLayerNames = validationLayers.data();                        // The names of the validation layers
		
		// Populate the debug create info
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
										  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
										  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
									  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
									  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
		debugCreateInfo.pUserData = nullptr;  // Optional
		
		// Give the struct the creation info.
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
	} else
	{
		// Specify that no layers are enabled (0)
		createInfo.enabledLayerCount = 0;
		
		// Give it a nullptr to the creation info.
		createInfo.pNext = nullptr;
	}
	
	// Finally, create the instance_
	if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
	{
		// If the instance failed to be created, throw a runtime error.
		throw std::runtime_error("failed to create Vulkan instance!");
	}
	
	has_glfw_required_instance_extensions();
}

/// <summary>
/// Method for checking if the physical device supports the required extensions.
/// </summary>
/// <param name="device">: The physical device to check. </param>
/// <returns>True if the device supports all of the extendsions. False otherwise. </returns>
bool Device::check_device_extension_support(VkPhysicalDevice device)
{
	// Get the extension count
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	
	// Get the avaliable extensions
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	
	// Make a string set of the required extensions
	std::set<std::string> requiredExtensions{deviceExtensions.begin(), deviceExtensions.end()};
	
	// Erase any supported extensions from the set
	for (const auto &extension: availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}
	
	// Check if the set is empty. If it is, all of the extensions are supported.
	return requiredExtensions.empty();
}

bool Device::check_validation_layer_support()
{
	// Get the validation layer count
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	
	// Create a vector to store the available layers
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	// print the available layers
	{
		std::stringstream ss;
		
		ss << "\n available layers:";
		
		for (const auto &layer: availableLayers)
			ss << "\n\t" << layer.layerName;
		
		AITO_TRACE(ss.str());
	}
	
	// Iterate through each layername in the list of required layers
	for (const char *layerName: validationLayers)
	{
		bool layerFound = false;
		
		// Check if the layer is supported by iterating through the supported layers
		for (const auto &layerProperties: availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		
		// If any layer is not found. Return false.
		if (!layerFound)
		{
			AITO_ERROR("Layer was not found during Validation Layer Support checking! Layer name is: {}", layerName);
			return false;
		}
	}
	
	return true;
}

void Device::setup_debug_messenger()
{
	// Return if the debug messenger isn't nececary (if validation layers are not enabled).
	if (!enableValidationLayers)
		return;
	
	// Create the debug messenger create info.
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;  // Optional
	
	// Try to create the debug messenger. Throw a runtime error if it failed.
	if (CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger_) != VK_SUCCESS)
	{
		AITO_FATAL("Failed to set up debug messenger");
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

void Device::pick_physical_device()
{
	// Find the number of physical devices that support vulkan.
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
	
	// Check if any of the graphics cards support vulkan.
	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}
	
	// Store the physical handles in an array
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());
	
	
	// Use an ordered map to automatically sort candidates by increasing score
	std::multimap<uint32_t, VkPhysicalDevice> candidates;
	
	for (const auto &device: devices)
	{
		uint32_t score = rate_device_suitability(device);
		candidates.insert(std::make_pair(score, device));
	}
	
	// Get the physical properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(candidates.rbegin()->second, &deviceProperties);
	
	// Print the name of the chosen GPU
	AITO_INFO("Best GPU found is: {}", deviceProperties.deviceName);
	
	
	// Check if the best candidate is suitable at all
	if (candidates.rbegin()->first > 0)
	{
		physicalDevice_ = candidates.rbegin()->second;
		physicalDeviceProperties_ = deviceProperties;
	} else
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

uint32_t Device::rate_device_suitability(VkPhysicalDevice device)
{
	// First, check if the device is suitable at all.
	if (!is_device_suitable(device))
		return 0;
	
	// Get the device properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	
	// Get the device features
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	
	uint32_t score = 0;
	
	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}
	
	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;
	
	return score;
}

bool Device::is_device_suitable(VkPhysicalDevice device)
{
	// Get the device properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	
	// Get the device features
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	
	// Get the device queue families
	QueueFamilyIndices indices = find_queue_families(device);
	
	// Check for extension support
	bool extensionsSupported = check_device_extension_support(device);
	
	// Check for swap chain adequacy
	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = query_swap_chain_support(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
	
	return indices.is_complete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices Device::find_queue_families(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	// Logic to find queue family indices to populate struct with
	// Queue family count
	uint32_t queueFamilyCount = 0;
	// Get the number of queue families.
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	// Get queue family properties
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	
	// Find a queue family that supports "VK_QUEUE_GRAPHICS_BIT"
	int i = 0;
	for (const auto &queueFamily: queueFamilies)
	{
		VkBool32 presentSupport = false;
		// Get the surface support of the device
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
		
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		if (presentSupport)
		{
			indices.presentFamily = i;
		}
		if (indices.is_complete())
		{
			break;
		}
		
		i++;
	}
	
	return indices;
}

/// <summary>
/// Method for getting the swap chain support details based on the given physical device.
/// </summary>
/// <param name="device">: The physical device. </param>
/// <returns>The swapchain support details (SwapChainSupportDetails). </returns>
SwapChainSupportDetails Device::query_swap_chain_support(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	
	// Get the surface capabilities of the physical device
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);
	
	// Get the format count
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);
	
	// if the format count isn't 0, resize the format list and insert the format details.
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
	}
	
	// Get the present mode count
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);
	
	// if the present mode count isn't 0, resize the mode list and populate it.
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
	}
	
	return details;
}

void Device::create_surface()
{ window_.create_window_surface(instance_, &surface_); }

void Device::create_logical_device()
{
	// Get the queue family indices
	QueueFamilyIndices indices = find_queue_families(physicalDevice_);
	
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		// Set the queue priority
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	
	// Declare used device features
	VkPhysicalDeviceFeatures deviceFeatures{};
	
	// Declare the locial device create info
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	
	// Give it the queue create infos
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	
	// Give it the device feature information
	createInfo.pEnabledFeatures = &deviceFeatures;
	// Tell it how many extensions are enabled
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	// Tell it which extensions are enabled
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	
	// Enable seperate validation layers
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}
	
	// Try to create the logical device
	if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}
	
	// Set the graphics queue
	vkGetDeviceQueue(device_, indices.graphicsFamily.value(), 0, &graphicsQueue_);
	// Set the presentation queue
	vkGetDeviceQueue(device_, indices.presentFamily.value(), 0, &presentQueue_);
}

void Device::create_command_pool()
{
	// Get the supported queue families from the GPU.
			QueueFamilyIndices queueFamilyIndices = find_physical_queue_families();
	
	// Create the struct for creating the command pool
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	
	// Get the index for the graphics command pool.
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags =
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	
	// Try to create the command pool.
	if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

VkFormat Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
									 VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice_, format, &props);
		
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (
				tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}
	AITO_ERROR("Failed to find supported format");
	throw std::runtime_error("failed to find supported format!");
}

void Device::createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image,
								 VkDeviceMemory &imageMemory)
{
	if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}
	
	// Get the memory requirements.
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device_, image, &memRequirements);
	
	// Create the memory allocation info and populate it.
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	// Get the index of the memory.
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
	
	// Attempt to allocate the memory.
	if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		AITO_FATAL("Failed to allocate image memory");
		throw std::runtime_error("failed to allocate image memory!");
	}
	// Attempt to bind the memory to the image.
	if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to bind image memory!");
	}
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	// Get the memory properties of the physical device
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memProperties);
	
	// Iterate through the memory types retrieved
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		// If the typefilter matches and the memory type has the correct properties (set by flags), return the index to the memory type.
		if (
				(typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties
				)
		{
			return i;
		}
	}
	
	AITO_ERROR("Failed to find suitable memory type");
	throw std::runtime_error("failed to find suitable memory type!");
}
	
}