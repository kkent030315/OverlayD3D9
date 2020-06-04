#include "overlay.hpp"

bool overlay::Initialize(
	const HWND& window_handle,
	const COORD& back_buffer_size,
	const UINT& present_interval
)
{
	printf(XORSTR("[>] Entering %s\n"), __FUNCTION__);

	detail::d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (!detail::d3d9)
	{
		printf(XORSTR("[!] Failed to initialize D3D9 device.\n"));
		return false;
	}

	D3DPRESENT_PARAMETERS params;
	ZeroMemory(&params, sizeof(params));

	params.Windowed = true;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window_handle;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;

	params.BackBufferWidth = back_buffer_size.X;
	params.BackBufferHeight = back_buffer_size.Y;

	params.PresentationInterval = present_interval;

	params.EnableAutoDepthStencil = TRUE;
	params.AutoDepthStencilFormat = D3DFMT_D16;

	HRESULT result = detail::d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, 
		window_handle, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
		&params, 
		&detail::d3d9_device
	);

	if (FAILED(result))
	{
		printf(XORSTR("[!] Failed to create D3D9 device [0x%X]\n"), result);
		return false;
	}

	printf(XORSTR("[+] D3D9 device created at [0x%p]\n"), detail::d3d9_device);

	result = D3DXCreateLine(
		detail::d3d9_device, 
		&detail::d3d9_line
	);

	if (FAILED(result))
	{
		printf(XORSTR("[!] Failed to create D3D9 line [0x%X]\n"), result);
		return false;
	}

	printf(XORSTR("[+] D3D9 line created at [0x%p]\n"), detail::d3d9_device);

	result = D3DXCreateFont(
		detail::d3d9_device, 
		26, 
		NULL, 
		FW_REGULAR, 
		1, 
		FALSE, 
		DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, 
		ANTIALIASED_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, 
		TEXT("Meiryo UI"),
		&detail::d3d9_font
	);

	if (FAILED(result))
	{
		printf(XORSTR("[!] Failed to create D3D9 font [0x%X]\n"), result);
		return false;
	}

	printf(XORSTR("[+] D3D9 font created at [0x%p]\n"), detail::d3d9_device);

	printf(XORSTR("[<] Leaving %s\n"), __FUNCTION__);
	return true;
}

DWORD __stdcall overlay::OverlayRoutine(LPVOID arg)
{
	UNREFERENCED_PARAMETER(arg);

	MSG message = {};
	RECT rect = {};

	while (true)
	{
		ZeroMemory(&message, sizeof(MSG));

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (message.message == WM_QUIT)
		{
			exit(0);
		}

		OverlayRender();

		if (detail::overlay_routine_delay_ms > 0)
		{
			std::this_thread::sleep_for(
				std::chrono::milliseconds(detail::overlay_routine_delay_ms)
			);
		}
	}
}

void overlay::OverlayMainRoutine()
{
	constexpr auto color = D3DCOLOR_RGBA(255, 0, 0, 255);

	detail::current_tick_count = clock() * 0.001f;
	detail::overlay_framerate++;

	if ((detail::current_tick_count - detail::last_tick_count) > 1.0f)
	{
		detail::last_tick_count = detail::current_tick_count;
		detail::overlay_framerate_2 = detail::overlay_framerate;
		detail::overlay_framerate = 0;
	}

	renderer::DrawString(std::to_string(detail::overlay_framerate_2), { 15, 15 }, color, false, true, detail::d3d9_font);
	renderer::DrawString(XORSTR("TEST TEXT"), { 15, 45 }, color, false, true, detail::d3d9_font);

	renderer::DrawRectangle({ 1920 / 2, 1080 / 2, 60, 120 }, D3DCOLOR_ARGB(255, 155, 000, 155), true);
	renderer::DrawRectangle({ 1920 / 3, 1080 / 3, 60, 120 }, D3DCOLOR_ARGB(255, 155, 055, 155), true);
	renderer::DrawRectangle({ 1920 / 4, 1080 / 4, 60, 120 }, D3DCOLOR_ARGB(200, 055, 155, 155), true);
	renderer::DrawRectangle({ 1920 / 5, 1080 / 5, 60, 120 }, D3DCOLOR_ARGB(155, 055, 255, 155), true);
}

void overlay::OverlayRender()
{
	if (detail::d3d9)
	{
		detail::d3d9_device->Clear(0, NULL, D3DCLEAR_TARGET, renderer::colors::transparent, 1.0f, 0);
		detail::d3d9_device->BeginScene();

		OverlayMainRoutine();

		detail::d3d9_device->EndScene();
		detail::d3d9_device->Present(NULL, NULL, NULL, NULL);
	}
}

bool overlay::LaunchOverlay()
{
	if (detail::overlay_thread_id || detail::overlay_thread_handle)
	{
		return false;
	}

	detail::overlay_thread_handle = CreateThread(
		NULL,				// security attributes
		0,				// stack size
		OverlayRoutine,			// function
		NULL,				// augments
		0,				// option (0 or CREATE_SUSPENDED)
		&detail::overlay_thread_id	// [OUT] thread id
	);

	if (detail::overlay_thread_handle == INVALID_HANDLE_VALUE)
	{
		printf(XORSTR("[!] Failed to launch overlay [0x%lx]\n"), GetLastError());
		return false;
	}

	printf(
		XORSTR("[+] Overlay thread created. Handle: 0x%X ID: 0x%X\n"), 
		detail::overlay_thread_handle, detail::overlay_thread_id
	);

	return false;
}

bool overlay::TerminateOverlay()
{
	if (detail::overlay_thread_id && detail::overlay_thread_handle)
	{
		// return type is BOOL which is INT32 (typedef int)
		const auto result = TerminateThread(detail::overlay_thread_handle, 0) == TRUE ? true : false;

		if (!result)
		{
			printf(XORSTR("[!] Failed to terminate overlay [0x%lx]\n"), GetLastError());
			return false;
		}

		detail::overlay_thread_id = {};
		detail::overlay_thread_handle = {};
	}

	return false;
}

HWND overlay::CreateOverlayWindow(
	const WNDPROC& window_procedure,
	const std::string& class_name,
	const std::string& window_name,
	const RECT& rect
)
{
	printf(XORSTR("[>] Entering %s\n"), __FUNCTION__);

	WNDCLASSEX window_class;
	ZeroMemory(&window_class, sizeof(WNDCLASSEX));

	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = window_procedure;
	window_class.hInstance = GetModuleHandle(0);
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	window_class.lpszClassName = class_name.c_str();

	if (!RegisterClassEx(&window_class))
	{
		printf(XORSTR("[!] Failed to register window class [0x%lx]\n"), GetLastError());
		return NULL;
	}

	HWND result = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, // window style
		window_class.lpszClassName,		// window class
		window_name.c_str(),			// window class name
		WS_POPUP,				//window style
		rect.left,				// x position
		rect.top,				// y position
		std::abs(rect.right - rect.left),	// width
		std::abs(rect.top - rect.bottom),	// height
		NULL,					// parent window
		NULL,					// menu handle or control id
		window_class.hInstance,			// instance handle
		NULL					// CREATESTRUCT structure
	);

	if (result == NULL)
	{
		printf(XORSTR("[!] Failed to create window [0x%lx]\n"), GetLastError());
		return NULL;
	}

	printf(XORSTR("[+] New window has been created [0x%x]\n"), result);

	ShowWindow(result, SW_SHOW);
	SetImGUIControlable(false, result);

	printf(XORSTR("[<] Leaving %s\n"), __FUNCTION__);
	return result;
}

void overlay::SetImGUIControlable(
	bool controlable, 
	const HWND& window_handle
)
{
	if (controlable == true)
	{
		SetWindowLong(
			window_handle, 
			GWL_EXSTYLE, 
			WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT
		);

		SetLayeredWindowAttributes(window_handle, RGB(0, 0, 0), 0, ULW_COLORKEY);
		SetLayeredWindowAttributes(window_handle, 0, 255, LWA_ALPHA);
	}
	else
	{
		SetWindowLong(
			window_handle, 
			GWL_EXSTYLE, 
			WS_EX_LAYERED | WS_EX_TOOLWINDOW
		);

		SetLayeredWindowAttributes(window_handle, 0, 255, ULW_COLORKEY | LWA_ALPHA);
	}
}

RECT& overlay::renderer::GetTextDimension(
	const std::string& text, 
	const LPD3DXFONT& font
)
{
	RECT r;
	font->DrawTextA(NULL, text.data(), -1, &r, DT_CALCRECT, 0xFFFFFFFF);
	return r;
}

void overlay::renderer::DrawString(
	const std::string& text, 
	COORD position, 
	const unsigned long& color, 
	const bool& center, 
	const bool& outline, 
	const LPD3DXFONT& font
)
{
	if (center) 
	{
		const RECT& dimensions = GetTextDimension(text, font);
		position.X -= (dimensions.right - dimensions.left) / 2;
	}

	const auto lambda_draw_text = [&](std::string _text, int _x, int _y, unsigned long _color) {
		RECT r{ _x, _y, _x, _y };
		font->DrawTextA(NULL, _text.data(), -1, &r, DT_NOCLIP, _color);
	};

	if (outline) 
	{
		lambda_draw_text(text, position.X - 1, position.Y, colors::black);
		lambda_draw_text(text, position.X + 1, position.Y, colors::black);
		lambda_draw_text(text, position.X, position.Y - 1, colors::black);
		lambda_draw_text(text, position.X, position.Y + 1, colors::black);
	}

	lambda_draw_text(text, position.X, position.Y, color);
}

void overlay::renderer::DrawLine(
	const Vector2& position1, 
	const Vector2& position2, 
	const unsigned long& color
)
{
	D3DXVECTOR2 lines[2] = {
		D3DXVECTOR2(position1.x, position1.y),
		D3DXVECTOR2(position2.x, position2.y)
	};

	detail::d3d9_line->Begin();
	detail::d3d9_line->Draw(lines, 2, color);
	detail::d3d9_line->End();
}

void overlay::renderer::DrawRectangle(
	const Vector4& position,
	const unsigned long& color,
	const bool& outline
)
{
	const auto lambda_draw_rect = [](const Vector4& p, const unsigned long& c) {
		DrawLine({ p.x, p.y }, { p.x + p.z, p.y }, c);
		DrawLine({ p.x, p.y }, { p.x, p.y + p.w }, c);
		DrawLine({ p.x + p.z, p.y }, { p.x + p.z, p.y + p.w }, c);
		DrawLine({ p.x, p.y + p.w }, { p.x + p.z + 1, p.y + p.w }, c);
	};

	if (outline)
	{
		lambda_draw_rect({ position.x - 1, position.y - 1, position.z + 2, position.w + 2 }, colors::black);
		lambda_draw_rect({ position.x + 1, position.y + 1, position.z - 2, position.w - 2 }, colors::black);
	}

	lambda_draw_rect(position, color);
}

void overlay::renderer::DrawCircle(
	const Vector2& position, 
	const unsigned long& color, 
	const int& radius, 
	const int& num_sides
)
{
	D3DXVECTOR2 Line[128];
	float N = M_PI * 2.0 / num_sides;
	int count = 0;
	for (float iterator = 0; iterator < M_PI * 2.0; iterator += N)
	{
		float X1 = radius * cos(iterator) + position.x;
		float Y1 = radius * sin(iterator) + position.y;
		float X2 = radius * cos(iterator + N) + position.x;
		float Y2 = radius * sin(iterator + N) + position.y;
		Line[count].x = X1;
		Line[count].y = Y1;
		Line[count + 1].x = X2;
		Line[count + 1].y = Y2;
		count += 2;
	}
	detail::d3d9_line->Draw(Line, count, color);
}
