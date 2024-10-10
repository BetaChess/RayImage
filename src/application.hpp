#pragma once

#include <flowforge.hpp>
#include <vulkan/shader/default/material_shader.hpp>
#include <vulkan/shader/default/imgui_shader.hpp>

namespace rayimage
{

class Application
{
public:
	Application(uint32_t width, uint32_t height);
	~Application();

	void run();

private:
	uint32_t width_, height_;

	flwfrg::vk::Renderer renderer_;
	flwfrg::vk::shader::IMGuiShader im_gui_shader_;
	flwfrg::vk::shader::MaterialShader material_shader_;

	// Actual viewing "surface"
	std::array<flwfrg::vk::shader::MaterialShader::Vertex, 4> vertices_{};
	std::array<uint32_t, 6> indices_{};

	flwfrg::vk::Buffer vertex_buffer_;
	flwfrg::vk::Buffer index_buffer_;
	// id of the object (in the material shader) to render the drawn texture to.
	uint32_t object_id_ = flwfrg::vk::constant::invalid_id;
	flwfrg::vk::shader::GeometryRenderData object_data_{};
};

}
