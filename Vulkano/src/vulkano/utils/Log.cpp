#include <Windows.h>

#include "Log.hpp"

#define YELLOW 14

namespace vulkano
{
	Log& Log::get()
	{
		static Log s_inst;
		return s_inst;
	}

	void Log::set_colour()
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, static_cast<WORD>(YELLOW));
	}
} // namespace vulkano
