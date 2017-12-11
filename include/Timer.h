#pragma once

#include <chrono>
#include <ctime>

class Timer {
private:
	std::chrono::time_point<std::chrono::system_clock> startTime;
	std::chrono::time_point<std::chrono::system_clock> endTime;

	long long int elapsedTime = 0; //Microseconds

public:
	void start() { startTime = std::chrono::system_clock::now(); }
	void stop() { endTime = std::chrono::system_clock::now(); elapsedTime = 0; }
	void pause() { elapsedTime += getTimeInMicroSec(); stop(); }
	void unpause() { start(); }

	long long int getTimeInSec() { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startTime).count() + (elapsedTime/(static_cast<long long int>(1e6))); }
	long long int getTimeInMilliSec() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() + (elapsedTime / (static_cast<long long int>(1e3))); }
	long long int getTimeInMicroSec() { return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime).count() + elapsedTime; }
};
