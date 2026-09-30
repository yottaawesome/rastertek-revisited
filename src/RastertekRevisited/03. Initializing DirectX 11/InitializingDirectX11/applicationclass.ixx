////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module demo:applicationclass;
import win32;
import :d3dclass;

/////////////
// GLOBALS //
/////////////
constexpr auto FULL_SCREEN = false;
constexpr auto VSYNC_ENABLED = true;
constexpr auto SCREEN_DEPTH = 1000.0f;
constexpr auto SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	auto Initialize(int screenWidth, int screenHeight, HWND hwnd) -> bool
	{
		// Create and initialize the Direct3D object.
		m_Direct3D = new D3DClass;

		bool result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
		if (!result)
		{
			MessageBoxW(hwnd, L"Could not initialize Direct3D", L"Error", MB::Ok);
			return false;
		}

		return true;
	}

	void Shutdown()
	{
		// Release the Direct3D object.
		if (m_Direct3D)
		{
			m_Direct3D->Shutdown();
			delete m_Direct3D;
			m_Direct3D = 0;
		}
	}

	auto Frame() -> bool
	{
		// Render the graphics scene.
		if (!Render())
			return false;

		return true;
	}

private:
	auto Render() -> bool
	{
		// Clear the buffers to begin the scene.
		m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

		// Present the rendered scene to the screen.
		m_Direct3D->EndScene();

		return true;
	}

private:
	D3DClass* m_Direct3D = nullptr;
};
