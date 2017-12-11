#pragma once

#include <algorithm>
#include <string>

std::string toLower(const std::string& str) {
	std::string lower = str;

	std::transform(lower.begin(), lower.end(), lower.begin(), tolower);

	return lower;
}