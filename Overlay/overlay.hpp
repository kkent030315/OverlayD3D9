#pragma once

#include <Windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <iostream>
#include <time.h>
#include <chrono>
#include <thread>

#include "xor.hpp"
#include "vectors.hpp"

namespace overlay
{
	namespace detail
	{
		inline LPDIRECT3D9 d3d9 = {};
		inline LPDIRECT3DDEVICE9 d3d9_device = {};
		inline LPD3DXLINE d3d9_line = {};
		inline LPD3DXFONT d3d9_font = {};

		inline bool is_imgui_attached = false;

		inline DWORD overlay_thread_id = {};
		inline HANDLE overlay_thread_handle = {};

		inline int overlay_routine_delay_ms = 5;

		inline int overlay_framerate = {};
		inline int overlay_framerate_2 = {};
		inline float last_tick_count = {};
		inline float current_tick_count = {};
	}

	bool Initialize(
		const HWND& window_handle, 
		const COORD& back_buffer_size, 
		const UINT& present_interval
	);

	DWORD __stdcall OverlayRoutine(LPVOID arg);
	void OverlayMainRoutine();
	void OverlayRender();

	bool LaunchOverlay();
	bool TerminateOverlay();
	
	HWND CreateOverlayWindow(
		const WNDPROC& window_procedure, 
		const std::string& class_name, 
		const std::string& window_name, 
		const RECT& rect
	);

	void SetImGUIControlable(
		bool controlable, 
		const HWND& window_handle
	);

	namespace renderer
	{
		namespace colors
		{
			constexpr auto transparent = D3DCOLOR_RGBA(0, 0, 0, 0);
			constexpr auto black = D3DCOLOR_RGBA(1, 1, 1, 255);
		}

		RECT& GetTextDimension(
			const std::string& text, 
			const LPD3DXFONT& font
		);

		void DrawString(
			const std::string& text, 
			COORD position,
			const unsigned long& color, 
			const bool& center, 
			const bool& outline, 
			const LPD3DXFONT& font
		);

		void DrawLine(
			const Vector2& position1,
			const Vector2& position2,
			const unsigned long& color
		);

		void DrawRectangle(
			const Vector4& rect, 
			const unsigned long& color,
			const bool& outline
		);

		void DrawCircle(
			const Vector2& position, 
			const unsigned long& color, 
			const int& radius, 
			const int& num_sides
		);
	}
}