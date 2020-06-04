
#include <iostream>


#include "xor.hpp"
#include "overlay.hpp"
#include "utils.hpp"

#include <dwmapi.h>

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		break;
	case WM_CREATE:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int main()
{
	SetConsoleTitle(TEXT(""));

	HWND hwnd = overlay::CreateOverlayWindow(
		WindowProcedure,			// window procedure
		utils::RandomString(10),	// class name
		utils::RandomString(10),	// window name
		{ 0, 0, 1920, 1080 }		// window rect
	);

	overlay::Initialize(
		hwnd,							// window handle
		{ 1920, 1080 },					// back buffer size
		D3DPRESENT_INTERVAL_IMMEDIATE	// present interval
	);

	/* launch the overlay asynchronously */
	if (!overlay::LaunchOverlay())
	{
		printf(XORSTR("[!] Failed to launch the overlay."));
		return -1;
	}

    std::cin.get();
}