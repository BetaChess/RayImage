#pragma once

#include "renderer.hpp"


#include <flowforge.hpp>
#include <vulkan/resource/mutable_texture.hpp>
#include <vulkan/shader/default/imgui_shader.hpp>
#include <vulkan/shader/default/material_shader.hpp>

namespace rayimage
{

class Application
{
public:
	Application(uint32_t width, uint32_t height);
	~Application();

	void run();

	void update_texture();

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

	flwfrg::vk::MutableTexture texture_{};
	std::vector<uint8_t> image_data_{};

	Renderer ray_renderer_;
};

}
