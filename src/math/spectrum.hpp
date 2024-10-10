#pragma once

#include <array>
#include <bits/ranges_algobase.h>
#include <cstdint>

namespace rayimage
{

template<uint32_t sample_count>
class CoefficientSpectrum
{
public:
	explicit CoefficientSpectrum(float value = 0)
	{
		std::ranges::fill(c, value);
	}



	// Operators

	CoefficientSpectrum& operator+=(const CoefficientSpectrum &other)
	{
		for (size_t i = 0; i < sample_count; i++)
			c[i] += other.c[i];

		return *this;
	}
	CoefficientSpectrum operator+(const CoefficientSpectrum &other) const
	{
		CoefficientSpectrum result = *this;
		result += other;
		return result;
	}
	CoefficientSpectrum& operator-=(const CoefficientSpectrum &other)
	{
		for (size_t i = 0; i < sample_count; i++)
			c[i] -= other.c[i];

		return *this;
	}
	CoefficientSpectrum operator-(const CoefficientSpectrum &other) const
	{
		CoefficientSpectrum result = *this;
		result -= other;
		return result;
	}
	CoefficientSpectrum& operator*=(const CoefficientSpectrum &other)
	{
		for (size_t i = 0; i < sample_count; i++)
			c[i] *= other.c[i];

		return *this;
	}
	CoefficientSpectrum operator*(const CoefficientSpectrum &other) const
	{
		CoefficientSpectrum result = *this;
		result *= other;
		return result;
	}
	CoefficientSpectrum& operator*=(float scalar)
	{
		for (size_t i = 0; i < sample_count; i++)
			c[i] *= scalar;

		return *this;
	}
	CoefficientSpectrum operator*(float scalar) const
	{
		CoefficientSpectrum result = *this;
		result *= scalar;
		return result;
	}
	CoefficientSpectrum& operator/=(const CoefficientSpectrum &other)
	{
		for (size_t i = 0; i < sample_count; i++)
			c[i] /= other.c[i];

		return *this;
	}
	CoefficientSpectrum operator/(const CoefficientSpectrum &other) const
	{
		CoefficientSpectrum result = *this;
		result /= other;
		return result;
	}
	CoefficientSpectrum operator-() const
	{
		CoefficientSpectrum result;
		for (size_t i = 0; i < sample_count; i++)
			result.c[i] = -c[i];

		return result;
	}

	// Equality

	bool operator==(const CoefficientSpectrum &other) const
	{
		for (size_t i = 0; i < sample_count; i++)
			if (c[i] != other.c[i])
				return false;

		return true;
	}
	bool operator!=(const CoefficientSpectrum &other) const
	{
		for (size_t i = 0; i < sample_count; i++)
			if (c[i] != other.c[i])
				return true;

		return false;
	}

	// Access

	float operator[](size_t i) const
	{
		return c[i];
	}
	float& operator[](size_t i)
	{
		return c[i];
	}


protected:
	std::array<float, sample_count> c;
};
template<uint32_t sample_count>
inline CoefficientSpectrum<sample_count> operator*(float scalar, const CoefficientSpectrum<sample_count> &s)
{
	return s * scalar;
}


typedef CoefficientSpectrum<3> RGBSpectrum;
typedef RGBSpectrum Spectrum;

inline Spectrum lerp(float t, const Spectrum &s1, const Spectrum &s2)
{
	return (1 - t) * s1 + t * s2;
}

}