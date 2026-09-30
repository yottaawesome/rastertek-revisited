////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module demo:systemclass;
///////////////////////
// MY CLASS INCLUDES //
///////////////////////
import win32;
import :inputclass;
import :applicationclass;

////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
export class SystemClass
{
public:
	/////////////
	// GLOBALS //
	/////////////
	static inline SystemClass* ApplicationHandle = nullptr;

	~SystemClass()
	{
		Shutdown();
	}

	auto Initialize() -> bool
	{
		// Initialize the width and height of the screen to zero before sending the variables into the function.
		auto screenWidth = 0;
		auto screenHeight = 0;

		// Initialize the windows api.
		InitializeWindows(screenWidth, screenHeight);

		// Create and initialize the application class object.  This object will handle rendering all the graphics for this application.
		if (not m_Application.Initialize(screenWidth, screenHeight, m_hwnd))
			return false;

		return true;
	}

	void Shutdown()
	{
		// Release the application class object.
		m_Application.Shutdown();

		// Shutdown the window.
		ShutdownWindows();
	}

	void Run()
	{
		auto msg = MSG{};
		auto done = false;
		auto result = false;

		// Loop until there is a quit message from the window or the user.
		while (not done)
		{
			// Handle the windows messages.
			if (PeekMessageW(&msg, nullptr, 0, 0, PM::Remove))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}

			// If windows signals to end the application then exit out.
			if (msg.message == WM::Quit)
			{
				done = true;
			}
			else
			{
				// Otherwise do the frame processing.
				result = Frame();
				if (not result)
					done = true;
			}
		}
	}

	auto MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) -> LRESULT
	{
		switch (umsg)
		{
			// Check if a key has been pressed on the keyboard.
			case WM::KeyDown:
			{
				// If a key is pressed send it to the input object so it can record that state.
				m_Input.KeyDown((unsigned int)wparam);
				return 0;
			}

			// Check if a key has been released on the keyboard.
			case WM::KeyUp:
			{
				// If a key is released then send it to the input object so it can unset the state for that key.
				m_Input.KeyUp((unsigned int)wparam);
				return 0;
			}

			// Any other messages send to the default message handler as our application won't make use of them.
			default:
			{
				return DefWindowProcW(hwnd, umsg, wparam, lparam);
			}
		}
	}

private:
	auto Frame() -> bool
	{
		// Check if the user pressed escape and wants to exit the application.
		if (m_Input.IsKeyDown(VK::Escape))
			return false;

		// Do the frame processing for the application class object.
		if (not m_Application.Frame())
			return false;

		return true;
	}

	void InitializeWindows(int& screenWidth, int& screenHeight)
	{
		// Get an external pointer to this object.	
		ApplicationHandle = this;

		// Get the instance of this application.
		m_hinstance = GetModuleHandleW(nullptr);

		// Give the application a name.
		m_applicationName = L"Engine";

		// Setup the windows class with default settings.
		auto wc = WNDCLASSEX{
			.cbSize = sizeof(WNDCLASSEX),
			.style = CS::HRedraw | CS::VRedraw | CS::OwnDC,
			.lpfnWndProc = WndProc,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = m_hinstance,
			.hIcon = LoadIconW(nullptr, IDI::WinLogo),
			.hCursor = LoadCursorW(nullptr, IDC::Arrow),
			.hbrBackground = (HBRUSH)GetStockObject(Brushes::Black),
			.lpszMenuName = nullptr,
			.lpszClassName = m_applicationName,
			.hIconSm = LoadIconW(nullptr, IDI::WinLogo)
		};

		// Register the window class.
		RegisterClassExW(&wc);

		// Determine the resolution of the clients desktop screen.
		screenWidth = GetSystemMetrics(SM::CXScreen);
		screenHeight = GetSystemMetrics(SM::CYScreen);

		// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		auto posX = 0, posY = 0;
		if (FULL_SCREEN)
		{
			// If full screen set the screen to maximum size of the users desktop and 32bit.
			auto dmScreenSettings = DEVMODE{
				.dmSize = sizeof(DEVMODE),
				.dmFields = DM::PelsWidth | DM::PelsHeight | DM::BitsPerPel,
				.dmBitsPerPel = 32,
				.dmPelsWidth = (unsigned long)screenWidth,
				.dmPelsHeight = (unsigned long)screenHeight,
			};

			// Change the display settings to full screen.
			ChangeDisplaySettingsW(&dmScreenSettings, CDS::Fullscreen);

			// Set the position of the window to the top left corner.
			posX = posY = 0;
		}
		else
		{
			// If windowed then set it to 800x600 resolution.
			screenWidth = 800;
			screenHeight = 600;

			// Place the window in the middle of the screen.
			posX = (GetSystemMetrics(SM::CXScreen) - screenWidth) / 2;
			posY = (GetSystemMetrics(SM::CYScreen) - screenHeight) / 2;
		}

		// Create the window with the screen settings and get the handle to it.
		m_hwnd = CreateWindowExW(
			WsEx::AppWindow, 
			m_applicationName, 
			m_applicationName,
			Ws::ClipSiblings | Ws::ClipChildren | Ws::Popup,
			posX, 
			posY, 
			screenWidth, 
			screenHeight, 
			nullptr, 
			nullptr, 
			m_hinstance, 
			nullptr
		);

		// Bring the window up on the screen and set it as main focus.
		ShowWindow(m_hwnd, SW::Show);
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);

		// Hide the mouse cursor.
		ShowCursor(false);
	}

	void ShutdownWindows()
	{
		if (not m_hwnd)
			return;

		// Show the mouse cursor.
		ShowCursor(true);

		// Fix the display settings if leaving full screen mode.
		if (FULL_SCREEN)
			ChangeDisplaySettingsW(nullptr, 0);

		// Remove the window.
		DestroyWindow(m_hwnd);
		m_hwnd = nullptr;

		// Remove the application instance.
		UnregisterClassW(m_applicationName, m_hinstance);
		m_hinstance = nullptr;

		// Release the pointer to this class.
		ApplicationHandle = nullptr;
	}

	static auto WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) -> LRESULT
	{
		switch (umessage)
		{
				// Check if the window is being destroyed.
			case WM::Destroy:
			{
				PostQuitMessage(0);
				return 0;
			}

			// Check if the window is being closed.
			case WM::Close:
			{
				PostQuitMessage(0);
				return 0;
			}

			// All other messages pass to the message handler in the system class.
			default:
			{
				return SystemClass::ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
			}
		}
	}

private:
	LPCWSTR m_applicationName = L"";
	HINSTANCE m_hinstance = nullptr;
	HWND m_hwnd = nullptr;

	InputClass m_Input;
	ApplicationClass m_Application;
};
