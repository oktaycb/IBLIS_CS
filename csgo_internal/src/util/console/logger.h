#pragma once

#include <source_location>
#include <format>

#undef ERROR
//#define ALLOC_CONSOLE

#ifdef ALLOC_CONSOLE
enum class LogType_t 
{
	INFO = 0,
	SUCCESS,
	WARNING,
	ERROR,
	FATAL
};

template <LogType_t LogType, typename T, typename ...P>
__forceinline void print(const std::source_location& loc, T&&msg, P&&... params)
{
	std::cout << "\x1b[0m";
	std::string file_name = loc.file_name();
	file_name.erase(0, file_name.find_last_of('\\') + 1);

	const auto fmtloc = std::format("[!] [ {}({}): {} ] ", file_name, loc.line(), loc.function_name());

	switch (LogType)
	{
	case LogType_t::INFO: std::cout << fmtloc << std::vformat(std::forward<T>(msg), std::make_format_args(std::forward<P>(params)...)); break;
	case LogType_t::SUCCESS: std::cout << "\x1b[32;1m" << fmtloc << std::vformat(std::forward<T>(msg), std::make_format_args(std::forward<P>(params)...)); break;
	case LogType_t::WARNING: std::cout << "\x1b[43;1m" << fmtloc << std::vformat(std::forward<T>(msg), std::make_format_args(std::forward<P>(params)...)); break;
	case LogType_t::ERROR: std::cout << "\x1b[41;1m" << fmtloc << std::vformat(std::forward<T>(msg), std::make_format_args(std::forward<P>(params)...)); break;
	case LogType_t::FATAL: std::cout << "\x1b[45;1m" << fmtloc << std::vformat(std::forward<T>(msg), std::make_format_args(std::forward<P>(params)...)); break;
	}
}

#define LOG_INFO(str, ...) print<LogType_t::INFO>(std::source_location::current(), str, __VA_ARGS__)
#define LOG_SUCCESS(str, ...) print<LogType_t::SUCCESS>(std::source_location::current(), str, __VA_ARGS__)
#define LOG_WARNING(str, ...) print<LogType_t::WARNING>(std::source_location::current(), str, __VA_ARGS__)
#define LOG_ERROR(str, ...) print<LogType_t::ERROR>(std::source_location::current(), str, __VA_ARGS__)
#define LOG_FATAL(str, ...) print<LogType_t::FATAL>(std::source_location::current(), str, __VA_ARGS__)
#else
#define LOG_INFO(...)
#define LOG_SUCCESS(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)
#endif