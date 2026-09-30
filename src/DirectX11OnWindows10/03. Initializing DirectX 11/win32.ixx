module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <directxmath.h>

export module win32;

// Basic Windows types and functions
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
		::LPVOID,
		::MSG,
		::WPARAM,
		::LPARAM,
		::HRESULT,
		::HINSTANCE,
		::DEVMODE,
		::WNDCLASSEX,
		::HBRUSH,
		::LPCWSTR,
		::PSTR,
		::wcstombs_s,
		::strcpy_s,
		::MessageBoxW,
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

	constexpr auto Failed(HRESULT hr) noexcept { return FAILED(hr); }

	namespace MB
	{
		enum
		{
			Ok = MB_OK,
		};
	}

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

// D3D
export using
	::D3D_FEATURE_LEVEL,
	::D3D_DRIVER_TYPE
	;

// DXGI
export 
{
	using
		::IDXGISwapChain,
		::IDXGIFactory,
		::IDXGIAdapter,
		::IDXGIOutput,
		::DXGI_MODE_DESC,
		::DXGI_ADAPTER_DESC,
		::DXGI_SWAP_CHAIN_DESC,
		::DXGI_FORMAT,
		::CreateDXGIFactory
		;
	namespace DxgiEnumModes
	{
		enum
		{
			Interlaced = DXGI_ENUM_MODES_INTERLACED
		};
	};

	namespace DxgiUsage
	{
		enum
		{
			RenderTargetOutput = DXGI_USAGE_RENDER_TARGET_OUTPUT
		};
	}
}

// D3D11
export
{
	using
		::ID3D11Device,
		::ID3D11DeviceContext,
		::ID3D11RenderTargetView,
		::ID3D11Texture2D,
		::ID3D11DepthStencilState,
		::ID3D11DepthStencilView,
		::ID3D11RasterizerState,
		::D3D11_CLEAR_FLAG,
		::DXGI_SWAP_EFFECT,
		::D3D11_BIND_FLAG,
		::D3D11_FILL_MODE,
		::D3D11_CULL_MODE,
		::D3D11_TEXTURE2D_DESC,
		::D3D11_DSV_DIMENSION,
		::D3D11_DEPTH_STENCIL_DESC,
		::D3D11_DEPTH_STENCIL_VIEW_DESC,
		::D3D11_VIEWPORT,
		::D3D11_RASTERIZER_DESC,
		::D3D11_USAGE,
		::D3D11_DEPTH_WRITE_MASK,
		::D3D11_COMPARISON_FUNC,
		::D3D11_STENCIL_OP,
		::D3D11CreateDeviceAndSwapChain
		;

	constexpr auto D3d11SdkVersion = D3D11_SDK_VERSION;
}

export namespace DirectX
{
	using 
		::DirectX::XMMATRIX,
		::DirectX::XMMatrixPerspectiveFovLH,
		::DirectX::XMMatrixIdentity,
		::DirectX::XMMatrixOrthographicLH
		;
}
