#pragma once

#include "def.hh"

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <future>
#include <queue>
#include <cmath>

constexpr uint8_t ui8Max = std::numeric_limits<uint8_t>::max();
constexpr uint16_t ui16Max = std::numeric_limits<uint16_t>::max();
constexpr float ui16Maxf = static_cast<float>(std::numeric_limits<uint16_t>::max());

void writeScreenshot(std::string const &folderPath, uint32_t width, uint32_t height);

inline double round1P(double val)
{
	return std::round(val * 10.0) / 10.0;
}

inline double round2P(double val)
{
	return std::round(val * 100.0) / 100.0;
}

inline double round3P(double val)
{
	return std::round(val * 1000.0) / 1000.0;
}

inline double round4P(double val)
{
	return std::round(val * 1000.0) / 1000.0;
}
