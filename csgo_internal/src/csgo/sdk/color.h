#pragma once

class Color {
public:
	std::array<uint8_t, 4> m_Color;

	Color() = default;
	~Color() = default;

	Color(const int r, const int g, const int b, const int a = 255) : m_Color({ 0, 0, 0, 0 })
	{
		set(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	}

	Color(const float r, const float g, const float b, const float a = 255.f) : m_Color({ 0, 0, 0, 0 })
	{
		set(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	}

	Color(Color color, const int alpha) 
	{ 
		set(color.r() / 255.f, color.g() / 255.f, color.b() / 255.f, alpha);
	}

	void set(const float r, const float g, const float b, const float a)
	{
		m_Color.at(0) = static_cast<uint8_t> (r * 255);
		m_Color.at(1) = static_cast<uint8_t> (g * 255);
		m_Color.at(2) = static_cast<uint8_t> (b * 255);
		m_Color.at(3) = static_cast<uint8_t> (a * 255);
	}

	Color OverrideAlpha(int alpha)
	{
		return Color(static_cast<int>(r()), static_cast<int>(g()), static_cast<int>(b()), alpha);
	}

	Color ToFraction()
	{
		return Color(r() / 255.f, g() / 255.f, b() / 255.f, a() / 255.f);
	}

	__forceinline uint8_t& r() { return m_Color.at(0); }
	__forceinline uint8_t& g() { return m_Color.at(1); }
	__forceinline uint8_t& b() { return m_Color.at(2); }
	__forceinline uint8_t& a() { return m_Color.at(3); }

	static Color FromHSB(float hue, float saturation, float brightness) {
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1) {
			return Color(
				static_cast<uint8_t>(brightness * 255),
				static_cast<uint8_t>(t * 255),
				static_cast<uint8_t>(p * 255)
			);
		}
		else if (h < 2) {
			return Color(
				static_cast<uint8_t>(q * 255),
				static_cast<uint8_t>(brightness * 255),
				static_cast<uint8_t>(p * 255)
			);
		}
		else if (h < 3) {
			return Color(
				static_cast<uint8_t>(p * 255),
				static_cast<uint8_t>(brightness * 255),
				static_cast<uint8_t>(t * 255)
			);
		}
		else if (h < 4) {
			return Color(
				static_cast<uint8_t>(p * 255),
				static_cast<uint8_t>(q * 255),
				static_cast<uint8_t>(brightness * 255)
			);
		}
		else if (h < 5) {
			return Color(
				static_cast<uint8_t>(t * 255),
				static_cast<uint8_t>(p * 255),
				static_cast<uint8_t>(brightness * 255)
			);
		}
		else {
			return Color(
				static_cast<uint8_t>(brightness * 255),
				static_cast<uint8_t>(p * 255),
				static_cast<uint8_t>(q * 255)
			);
		}
	}
};

namespace colors {
	static Color white{ 255.f, 255.f, 255.f, 255.f };
	static Color black{ 0, 0, 0, 255 };
	static Color yellowgreen{ 182,231,23,255 };
	static Color red{ 255, 0, 0, 255 };
	static Color burgundy{ 52, 155, 235 };
	static Color light_blue{ 95, 174, 227, 255 };
	static Color orange{ 243, 156, 18, 255 };
	static Color green{ 149, 184, 6, 255 };
	static Color purple{ 180, 60, 120, 255 };
	static Color transparent_green{ 0, 255, 0, 200 };
	static Color transparent_yellow{ 255, 255, 0, 200 };
	static Color transparent_red{ 255, 0, 0, 200 };
}