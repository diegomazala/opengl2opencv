#ifndef _TIME_H_
#define _TIME_H_

#include <chrono>
#include <iostream>

std::chrono::steady_clock::time_point startTime;
static double diff_msec(const std::chrono::steady_clock::time_point& start_time)
{
	return std::chrono::duration<double, std::milli>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start_time)
		).count();
}

static void beginTime() 
{
	startTime = std::chrono::steady_clock::now();
}

static void endTime(const char* msg)
{
	std::cout << msg << ' ' << diff_msec(startTime) << std::endl;
}

static double diffTime()
{
	return diff_msec(startTime);
}

#endif // !_TIME_H_