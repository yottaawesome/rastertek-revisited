/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
import std;
import win32;
import demo;

auto WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) -> int
{
	// Create the system object.
	auto System = new SystemClass;
	// Initialize and run the system object.
	if (System->Initialize())
		System->Run();
	// Shutdown and release the system object.
	System->Shutdown();
	return 0;
}