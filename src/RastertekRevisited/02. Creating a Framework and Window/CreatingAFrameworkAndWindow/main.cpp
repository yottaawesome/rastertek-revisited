////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
import frameworkandwindow;
import win32;

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
