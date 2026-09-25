////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module frameworkandwindow:applicationclass;
import win32;

/////////////
// GLOBALS //
/////////////
constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	auto Initialize(int screenWidth, int screenHeight, HWND hwnd) -> bool
	{
		return true;
	}

	void Shutdown()
	{

	}

	auto Frame() -> bool
	{
		return true;
	}

private:
	auto Render() -> bool
	{
		return true;
	}

private:

};
