#pragma once

#include "math/spectrum.hpp"

#include <glm/glm.hpp>

namespace rayimage
{

class Renderer
{
public:
	Renderer(uint32_t width, uint32_t height);
	~Renderer() = default;

	void render();

	const std::vector<RGBSpectrum>& get_image() const
	{
		return image_;
	}

private:
	uint32_t width_, height_;

	std::vector<RGBSpectrum> image_;

	static RGBSpectrum per_pixel(glm::vec2 coord);
};

}
