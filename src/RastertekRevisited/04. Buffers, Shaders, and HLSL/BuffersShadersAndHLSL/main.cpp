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

auto WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) -> int
{
	// Create the system object.
	auto System = new SystemClass;

	// Initialize and run the system object.
	auto result = System->Initialize();
	if(result)
		System->Run();

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}
