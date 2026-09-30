module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

export module win32;

export
{
	template<auto VConstant>
	struct Win32Constant
	{
		static constexpr auto operator()() { return VConstant; }
		constexpr operator decltype(VConstant)() const{ return VConstant; }
	};

	using
		::LRESULT,
		::HWND,
		::UINT,
		::MSG,
		::WPARAM,
		::LPARAM,
		::HINSTANCE,
		::DEVMODE,
		::WNDCLASSEX,
		::HBRUSH,
		::LPCWSTR,
		::PSTR,
		::RegisterClassExW,
		::ShowWindow,
		::SetForegroundWindow,
		::SetFocus,
		::CreateWindowExW,
		::ShowCursor,
		::DestroyWindow,
		::GetSystemMetrics,
		::ChangeDisplaySettingsW,
		::RegisterClassExW,
		::UnregisterClassW,
		::GetStockObject,
		::PostQuitMessage,
		::LoadCursorW,
		::LoadIconW,
		::GetModuleHandleW,
		::DefWindowProcW,
		::PeekMessageW,
		::TranslateMessage,
		::DispatchMessageW
		;

	namespace SW
	{
		enum
		{
			Show = SW_SHOW,
		};
	}

	namespace WsEx
	{
		enum
		{
			AppWindow = WS_EX_APPWINDOW,
		};
	}

	namespace Ws
	{
		enum
		{
			ClipSiblings = WS_CLIPSIBLINGS,
			ClipChildren = WS_CLIPCHILDREN,
			Popup = WS_POPUP,
		};
	}

	namespace CDS
	{
		enum
		{
			Fullscreen = CDS_FULLSCREEN,
		};
	}

	namespace DM
	{
		enum
		{
			PelsWidth = DM_PELSWIDTH,
			PelsHeight = DM_PELSHEIGHT,
			DisplayFlags = DM_DISPLAYFLAGS,
			DisplayFrequency = DM_DISPLAYFREQUENCY,
			BitsPerPel = DM_BITSPERPEL,
		};
	}

	namespace SM
	{
		enum
		{
			CXScreen = SM_CXSCREEN,
			CYScreen = SM_CYSCREEN,
		};
	}

	namespace Brushes
	{
		constexpr auto Black = BLACK_BRUSH;
	}

	namespace IDI
	{
		constexpr auto WinLogo = Win32Constant<IDI_WINLOGO>{};
	}

	namespace IDC
	{
		constexpr auto Arrow = Win32Constant<IDC_ARROW>{};
	}

	namespace CS
	{
		enum
		{
			HRedraw = CS_HREDRAW,
			VRedraw = CS_VREDRAW,
			OwnDC = CS_OWNDC,
		};
	}

	namespace WM
	{
		enum 
		{
			Quit = WM_QUIT,
			KeyDown = WM_KEYDOWN,
			KeyUp = WM_KEYUP,
			Destroy = WM_DESTROY,
			Close = WM_CLOSE,
		};
	}

	namespace PM
	{
		enum
		{
			Remove = PM_REMOVE,
		};
	}

	namespace VK
	{
		enum
		{
			Escape = VK_ESCAPE,
		};
	}
}

