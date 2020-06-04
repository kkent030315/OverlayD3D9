#include "utils.hpp"

COORD utils::GetScreenSize()
{
	return { 
		static_cast<short>(GetSystemMetrics(SM_CXSCREEN)), 
		static_cast<short>(GetSystemMetrics(SM_CYSCREEN)) };
}

std::string utils::RandomString(int length)
{
	std::string result;

	std::generate_n(result.begin(), length, []() -> char {
		std::random_device rnd;
		std::mt19937 mt(rnd());
		return chars[mt() % (sizeof(chars) - 1)];
	});

	return result;
}

int utils::GetProcessId(const std::string& process_name)
{
	unique_handle snap_shot { CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0), &CloseHandle };

	if (snap_shot.get() == INVALID_HANDLE_VALUE)
	{
		printf("[!] Failed to create toolhelp32 snapshot [0x%lx]\n", GetLastError());
		return 0;
	}

	PROCESSENTRY32 process_entry { sizeof(PROCESSENTRY32) };

	for (Process32First(snap_shot.get(), &process_entry); Process32Next(snap_shot.get(), &process_entry); )
	{
		if (std::strcmp(process_name.data(), process_entry.szExeFile) == 0)
		{
			return process_entry.th32ProcessID;
		}
	}

	return 0;
}
