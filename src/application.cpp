#include "pch.hpp"

#include "application.hpp"

rayimage::Application::Application(uint32_t width, uint32_t height)
	: width_(width), height_(height_),
	  renderer_(width, height, "RayImage"),
	  im_gui_shader_(&renderer_.get_display_context()),
	  material_shader_(&renderer_.get_display_context()),
	  vertex_buffer_(&renderer_.get_display_context().get_device(),
					 sizeof(flwfrg::vk::shader::MaterialShader::Vertex) * vertices_.size(),
					 static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
					 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false),
	  index_buffer_(&renderer_.get_display_context().get_device(),
					sizeof(uint32_t) * indices_.size(),
					static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false)
{
	vertices_[0].position = glm::vec3{-1, 1, 0};
	vertices_[0].texture_coordinate = glm::vec2{0.0f, 0.0f};

	vertices_[1].position = glm::vec3{1, -1, 0};
	vertices_[1].texture_coordinate = glm::vec2{1.0f, 1.0f};

	vertices_[2].position = glm::vec3{-1, -1, 0};
	vertices_[2].texture_coordinate = glm::vec2{0.0f, 1.0f};

	vertices_[3].position = glm::vec3{1, 1, 0};
	vertices_[3].texture_coordinate = glm::vec2{1.0f, 0.0f};
	vertex_buffer_.upload_data(
			vertices_.data(), 0,
			sizeof(flwfrg::vk::shader::MaterialShader::Vertex) * vertices_.size(),
			renderer_.get_display_context().get_device().get_graphics_command_pool(),
			nullptr,
			renderer_.get_display_context().get_device().get_graphics_queue());

	indices_[0] = 0;
	indices_[1] = 1;
	indices_[2] = 2;
	indices_[3] = 0;
	indices_[4] = 3;
	indices_[5] = 1;

	index_buffer_.upload_data(
			indices_.data(), 0,
			sizeof(uint32_t) * indices_.size(),
			renderer_.get_display_context().get_device().get_graphics_command_pool(),
			nullptr,
			renderer_.get_display_context().get_device().get_graphics_queue());

	object_id_ = material_shader_.acquire_resources();
	object_data_.textures[0] = material_shader_.get_default_texture();
}

rayimage::Application::~Application()
{
	material_shader_.release_resources(object_id_);

	vkDeviceWaitIdle(renderer_.get_display_context().get_device().get_logical_device());
}

void rayimage::Application::run()
{
	while (!renderer_.should_close())
	{
		auto frame_data = renderer_.begin_frame();
		if (!frame_data.has_value())
			continue;

		im_gui_shader_.begin_frame();

		material_shader_.update_global_state(glm::mat4(1), glm::mat4(1));
		object_data_.model = glm::mat4(1);
		material_shader_.update_object(object_data_);

		VkDeviceSize offsets[1] = {0};
		vkCmdBindVertexBuffers(
				frame_data.value()->get_handle(),
				0, 1,
				vertex_buffer_.ptr(),
				offsets);
		vkCmdBindIndexBuffer(
				frame_data.value()->get_handle(),
				index_buffer_.get_handle(),
				0,
				VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(
				frame_data.value()->get_handle(),
				indices_.size(), 1,
				0, 0, 0);

		im_gui_shader_.end_frame(*frame_data.value());

		renderer_.end_frame();
	}
}