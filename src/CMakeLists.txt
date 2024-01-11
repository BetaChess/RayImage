cmake_minimum_required(VERSION 3.20)

set(SOURCES
	# Dear ImGui sources
	${IMGUI_SOURCES}
	# Project sources
	pch.hpp
	main.cpp
	core/logger.hpp
	core/logger.cpp
	vulkan/window.cpp
	vulkan/window.hpp
	vulkan/device.cpp
	vulkan/device.hpp
	application.hpp
	application.cpp
	vulkan/renderer.hpp
	vulkan/renderer.cpp
	vulkan/swapchain.hpp
	vulkan/swapchain.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Precompiled header
target_precompile_headers(${PROJECT_NAME}
						  PUBLIC pch.hpp
)

set_target_properties(${PROJECT_NAME}
					  PROPERTIES
					  CXX_STANDARD 20
					  CXX_STANDARD_REQUIRED YES
					  CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME}
						   PUBLIC $ENV{VULKAN_SDK}/include/
						   PUBLIC ../vendor/glfw/include/
						   PUBLIC ${IMGUI_DIR}/backends/
						   PUBLIC ${IMGUI_DIR}
						   PUBLIC ../vendor/spdlog/include
)

target_link_directories(${PROJECT_NAME}
						PRIVATE ../vendor/glfw/src
)

target_link_libraries(${PROJECT_NAME}
					  glfw
					  ${Vulkan_LIBRARY}
)

include_directories(.)

