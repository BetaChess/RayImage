#include "pch.hpp"

#include "renderer.hpp"

namespace rayimage
{

Renderer::Renderer(uint32_t width, uint32_t height)
	: width_{width}, height_{height}
{
	image_.resize(width * height);
}

void Renderer::render()
{
	for (uint32_t i = 0; i < height_; i++)
	{
		for (uint32_t j = 0; j < width_; j++)
		{
			glm::vec2 coord = {static_cast<float>(j) / static_cast<float>(width_),
							   static_cast<float>(i) / static_cast<float>(height_)};
			coord = coord * 2.0f - 1.0f;

			image_[j + width_ * i] = per_pixel(coord);
		}
	}
}

RGBSpectrum Renderer::per_pixel(glm::vec2 coord)
{
	RGBSpectrum p{0};

	glm::vec3 ray_origin{0.f, 0.0f, 2.0f};
	glm::vec3 ray_direction{coord.x, coord.y, -1.f};
	float sphere_radius = 0.5f;
	// ray_direction = normalize(ray_direction);

	float a = glm::dot(ray_direction, ray_direction);
	float b = 2.0f * glm::dot(ray_origin, ray_direction);
	float c = glm::dot(ray_origin, ray_origin) - sphere_radius * sphere_radius;

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant >= 0.0f)
	{
		p[0] = 0.5f;
		p[1] = 0.0f;
		p[2] = 0.5f;
	}

	return p;
}

}// namespace rayimage
