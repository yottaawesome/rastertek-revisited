////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
import win32;
import frameworkandwindow;

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
