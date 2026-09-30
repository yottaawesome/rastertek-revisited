////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module frameworkandwindow:applicationclass;
import win32;

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
