#pragma once

// modelled after the original valve 'developer 1' debug console
// https://github.com/LestaD/SourceEngine2007/blob/master/se2007/engine/console.cpp

class NotifyText {
public:
	std::string m_text;
	Color		m_color;
	float		m_time;

public:
	__forceinline NotifyText(const std::string& text, Color color, float time) : m_text{ text }, m_color{ color }, m_time{ time } {}
};

class Notify {
private:
	std::vector< std::shared_ptr< NotifyText > > m_notify_text;

public:
	__forceinline Notify() : m_notify_text{ } {}

	__forceinline void add(const std::string& text, Color color = colors::white, float time = 8.f, bool console = true, bool visual = true) {
		// modelled after 'CConPanel::AddToNotify'
		if (visual)
			m_notify_text.push_back(std::make_shared< NotifyText >(text, color, time));

		if (console)
			g_Client.print(text);
	}

	// modelled after 'CConPanel::DrawNotify' and 'CConPanel::ShouldDraw'
	void think() {
		int		x{ 8 }, y{ 5 }, size{ 15 + 1 };
		Color	color;
		float	left;

		// update lifetimes.
		for (size_t i{ }; i < m_notify_text.size(); ++i) {
			auto notify = m_notify_text[i];

			notify->m_time -= g_Csgo.m_Globals->m_FrameTime;

			if (notify->m_time <= 0.f) {
				m_notify_text.erase(m_notify_text.begin() + i);
				continue;
			}
		}

		// we have nothing to draw.
		if (m_notify_text.empty())
			return;

		// iterate entries.
		for (size_t i{ }; i < m_notify_text.size(); ++i) {
			auto notify = m_notify_text[i];

			left = notify->m_time;
			color = notify->m_color;

			if (left < .5f) {
				float f = left;
				Math::clamp(f, 0.f, .5f);

				f /= .5f;

				color.a() = (int)(f * 179.f);

				if (i == 0 && f < 0.2f)
					y -= size * (1.f - f / 0.2f);
			}

			else
				color.a() = 179;

			new_render.StringShadow({ x, y }, CRender::Fonts::m_menu_font, {10, 10, 10, 179}, color, notify->m_text);
			y += size;
		}
	}
};

inline Notify g_notify;