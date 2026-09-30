#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
import std;
import win32;
import demo;

// See https://www.rastertek.com/dx11win10tut02.html

auto WinMain(HINSTANCE, HINSTANCE, PSTR, int) -> int
{
	// Create the system object.
	auto System = SystemClass{};
	// Initialize and run the system object.
	if (System.Initialize())
		System.Run();
	// Shutdown and release the system object.
	System.Shutdown();

	return 0;
}