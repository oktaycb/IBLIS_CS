#pragma once

struct Weather_t
{
	bool m_bHasBeenCreated = false;
	IClientNetworkable* m_Networkable = nullptr;
};

class C_WeatherSystem
{
public:
	void Init(int Stage);
	void Reset();
	IClientNetworkable* CreateWeatherEntity();
private:
	Weather_t m_WeatherData = Weather_t();
};

inline C_WeatherSystem g_WeatherSystem;