#pragma once
#include <Windows.h>
#include <string>
#include <random>
#include <algorithm>
#include <TlHelp32.h>
#include <memory>

#include "xor.hpp"

namespace utils
{
	using unique_handle = std::unique_ptr<void, decltype(&CloseHandle)>;

	constexpr char chars[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	
	COORD GetScreenSize();
	std::string RandomString(int length);
	int GetProcessId(const std::string& process_name);
}