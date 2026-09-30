////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module demo:applicationclass;
import std;
import win32;
import :cameraclass;
import :modelclass;
import :lightshaderclass;
import :lightclass;
import :d3dclass;

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
	~ApplicationClass()
	{
		Shutdown();
	}

	auto Initialize(int screenWidth, int screenHeight, HWND hwnd) -> bool
	{
		// Create and initialize the Direct3D object.
		auto result = m_Direct3D.Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
		if (not result)
		{
			MessageBoxW(hwnd, L"Could not initialize Direct3D", L"Error", MB::Ok);
			return false;
		}

		// Create the camera object.

		// Set the initial position of the camera.
		m_Camera.SetPosition(0.0f, 0.0f, -5.0f);

		// Set the file name of the texture file that we will be loading.
		auto textureFilename = std::string{ "stone01.tga" };

		// Create and initialize the model object.
		result = m_Model.Initialize(m_Direct3D.GetDevice(), m_Direct3D.GetDeviceContext(), textureFilename.data());
		if (not result)
		{
			MessageBoxW(hwnd, L"Could not initialize the model object.", L"Error", MB::Ok);
			return false;
		}

		// Create and initialize the light shader object.
		result = m_LightShader.Initialize(m_Direct3D.GetDevice(), hwnd);

		if (not result)
		{
			MessageBoxW(hwnd, L"Could not initialize the light shader object.", L"Error", MB::Ok);
			return false;
		}

		// Create and initialize the light object.
		m_Light.SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Light.SetDirection(0.0f, 0.0f, 1.0f);

		return true;
	}

	void Shutdown()
	{

		// Release the light shader object.
		m_LightShader.Shutdown();

		// Release the model object.
		m_Model.Shutdown();

		// Release the Direct3D object.
		m_Direct3D.Shutdown();
	}

	auto Frame() -> bool
	{
		static auto rotation = 0.0f;

		// Update the rotation variable each frame.
		rotation -= 0.0174532925f * 0.1f;
		if (rotation < 0.0f)
			rotation += 360.0f;

		// Render the graphics scene.
		if (not Render(rotation))
			return false;

		return true;
	}

private:
	auto Render(float rotation) -> bool
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

		// Rotate the world matrix by the rotation value so that the triangle will spin.
		worldMatrix = DirectX::XMMatrixRotationY(rotation);

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Model.Render(m_Direct3D.GetDeviceContext());

		// Render the model using the light shader.
		auto result = m_LightShader.Render(
			m_Direct3D.GetDeviceContext(), 
			m_Model.GetIndexCount(), 
			worldMatrix, 
			viewMatrix, 
			projectionMatrix, 
			m_Model.GetTexture(),
			m_Light.GetDirection(), 
			m_Light.GetDiffuseColor()
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
	LightShaderClass m_LightShader;
	LightClass m_Light;
};
