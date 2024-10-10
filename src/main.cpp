#include "pch.hpp"

#include "input/keyboard_controller.hpp"
#include "math/camera.hpp"
#include "math/transform.hpp"
#include "vulkan/shader/vertex.hpp"


#include <../../src/default_shaders.hpp>

#include <iostream>

int main()
{
	flwfrg::init();
	flwfrg::vk::Renderer renderer{1280, 720, "TestName"};
	auto &display_context = renderer.get_display_context();
	flwfrg::vk::shader::IMGuiShader im_gui_shader(&renderer.get_display_context());
	flwfrg::vk::shader::MaterialShader material_shader(&renderer.get_display_context());

	std::array<flwfrg::vk::shader::MaterialShader::Vertex, 4> vertices{};
	float scale = 1.0f;
	vertices[0].position = glm::vec3{-1, 1, 0} * scale;
	vertices[0].texture_coordinate = glm::vec2{0.0f, 0.0f};

	vertices[1].position = glm::vec3{1, -1, 0} * scale;
	vertices[1].texture_coordinate = glm::vec2{1.0f, 1.0f};

	vertices[2].position = glm::vec3{-1, -1, 0} * scale;
	vertices[2].texture_coordinate = glm::vec2{0.0f, 1.0f};

	vertices[3].position = glm::vec3{1, 1, 0} * scale;
	vertices[3].texture_coordinate = glm::vec2{1.0f, 0.0f};

	std::array<uint32_t, 6> indices{};
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 1;

	flwfrg::vk::Buffer vertex_buffer{
			&display_context.get_device(),
			sizeof(flwfrg::vk::shader::MaterialShader::Vertex) * vertices.size(),
			static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false};
	vertex_buffer.upload_data(
			vertices.data(), 0,
			sizeof(flwfrg::vk::shader::MaterialShader::Vertex) * vertices.size(),
			display_context.get_device().get_graphics_command_pool(),
			nullptr,
			display_context.get_device().get_graphics_queue());

	flwfrg::vk::Buffer index_buffer{
			&display_context.get_device(),
			sizeof(uint32_t) * indices.size(),
			static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false};
	index_buffer.upload_data(
			indices.data(), 0,
			sizeof(uint32_t) * indices.size(),
			display_context.get_device().get_graphics_command_pool(),
			nullptr,
			display_context.get_device().get_graphics_queue());

	// Acquire resources
	auto object_id = material_shader.acquire_resources();
	flwfrg::vk::shader::GeometryRenderData object_data{};
	object_data.textures[0] = material_shader.get_default_texture();

	while (!renderer.should_close())
	{
		auto frame_data = renderer.begin_frame();
		if (!frame_data.has_value())
			continue;

		im_gui_shader.begin_frame();

		material_shader.update_global_state(glm::mat4(1), glm::mat4(1));
		object_data.model = glm::mat4(1);
		material_shader.update_object(object_data);

		VkDeviceSize offsets[1] = {0};
		vkCmdBindVertexBuffers(frame_data.value()->get_handle(), 0, 1, vertex_buffer.ptr(), offsets);
		vkCmdBindIndexBuffer(frame_data.value()->get_handle(), index_buffer.get_handle(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(frame_data.value()->get_handle(), indices.size(), 1, 0, 0, 0);

		ImGui::ShowDemoWindow();

		im_gui_shader.end_frame(*frame_data.value());

		renderer.end_frame();
	}

	material_shader.release_resources(object_id);

	vkDeviceWaitIdle(display_context.get_device().get_logical_device());


	return 0;
}