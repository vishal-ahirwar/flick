#include <constants/colors.hpp>
#include <cstdio>
#include <log/log.h>
#include <rt/rt.h>

RT::RT(const std::string& msg) : mMsg{msg} { mStart = std::chrono::high_resolution_clock::now(); };
RT::~RT()
{
	mEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapedTime{mEnd - mStart};
	auto formatedString = fmt::format("Time taken by {} {:.2f}s\n", mMsg.c_str(), elapedTime.count());
	Log::log(formatedString, Type::E_DISPLAY);
};