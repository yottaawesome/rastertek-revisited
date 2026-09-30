////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module demo:applicationclass;
import win32;
import :d3dclass;
import :cameraclass;
import :modelclass;
import :textureshaderclass;

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
	~ApplicationClass()
	{
		Shutdown();
	}

	auto Initialize(int screenWidth, int screenHeight, HWND hwnd) -> bool
	{
		auto result = m_Direct3D.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
		if (not result)
		{
			MessageBoxW(hwnd, L"Could not initialize Direct3D", L"Error", MB::Ok);
			return false;
		}

		// Set the initial position of the camera.
		m_Camera.SetPosition(0.0f, 0.0f, -5.0f);

		// Set the name of the texture file that we will be loading.
		auto textureFilename = std::string{ "stone01.tga" };

		result = m_Model.Initialize(m_Direct3D.GetDevice(), m_Direct3D.GetDeviceContext(), textureFilename);
		if (not result)
		{
			MessageBoxW(hwnd, L"Could not initialize the model object.", L"Error", MB::Ok);
			return false;
		}

		result = m_TextureShader.Initialize(m_Direct3D.GetDevice(), hwnd);
		if (not result)
		{
			MessageBoxW(hwnd, L"Could not initialize the texture shader object.", L"Error", MB::Ok);
			return false;
		}

		return true;
	}

	void Shutdown()
	{
		// Release the texture shader object.
		m_TextureShader.Shutdown();
		// Release the model object.
		m_Model.Shutdown();
		// Release the Direct3D object.
		m_Direct3D.Shutdown();
	}

	auto Frame() -> bool
	{
		// Render the graphics scene.
		if (not Render())
			return false;
		return true;
	}

private:
	auto Render() -> bool
	{
		// Clear the buffers to begin the scene.
		m_Direct3D.BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

		// Generate the view matrix based on the camera's position.
		m_Camera.Render();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		auto worldMatrix = DirectX::XMMATRIX{};
		m_Direct3D.GetWorldMatrix(worldMatrix);
		auto viewMatrix = DirectX::XMMATRIX{};
		m_Camera.GetViewMatrix(viewMatrix);
		auto projectionMatrix = DirectX::XMMATRIX{};
		m_Direct3D.GetProjectionMatrix(projectionMatrix);

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Model.Render(m_Direct3D.GetDeviceContext());

		// Render the model using the texture shader.
		auto result = m_TextureShader.Render(
			m_Direct3D.GetDeviceContext(), 
			m_Model.GetIndexCount(), 
			worldMatrix, 
			viewMatrix, 
			projectionMatrix, 
			m_Model.GetTexture()
		);
		if (not result)
			return false;

		// Present the rendered scene to the screen.
		m_Direct3D.EndScene();

		return true;
	}

private:
	D3DClass m_Direct3D;
	CameraClass m_Camera;
	ModelClass m_Model;
	TextureShaderClass m_TextureShader;
};
