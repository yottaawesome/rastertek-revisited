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
	auto Initialize(int screenWidth, int screenHeight, HWND hwnd) -> bool
	{
		// Create and initialize the Direct3D object.
		m_Direct3D = new D3DClass;

		auto result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
		if (not result)
		{
			MessageBoxW(hwnd, L"Could not initialize Direct3D", L"Error", MB::Ok);
			return false;
		}

		// Create the camera object.
		m_Camera = new CameraClass;

		// Set the initial position of the camera.
		m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

		// Set the file name of the texture file that we will be loading.
		auto textureFilename = std::string{ "stone01.tga" };

		// Create and initialize the model object.
		m_Model = new ModelClass;

		result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename.data());
		if (not result)
		{
			MessageBoxW(hwnd, L"Could not initialize the model object.", L"Error", MB::Ok);
			return false;
		}

		// Create and initialize the light shader object.
		m_LightShader = new LightShaderClass;

		result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
		if (!result)
		{
			MessageBoxW(hwnd, L"Could not initialize the light shader object.", L"Error", MB::Ok);
			return false;
		}

		// Create and initialize the light object.
		m_Light = new LightClass;

		m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Light->SetDirection(0.0f, 0.0f, 1.0f);

		return true;
	}

	void Shutdown()
	{
		// Release the light object.
		if (m_Light)
		{
			delete m_Light;
			m_Light = 0;
		}

		// Release the light shader object.
		if (m_LightShader)
		{
			m_LightShader->Shutdown();
			delete m_LightShader;
			m_LightShader = 0;
		}

		// Release the model object.
		if (m_Model)
		{
			m_Model->Shutdown();
			delete m_Model;
			m_Model = 0;
		}

		// Release the camera object.
		if (m_Camera)
		{
			delete m_Camera;
			m_Camera = 0;
		}

		// Release the Direct3D object.
		if (m_Direct3D)
		{
			m_Direct3D->Shutdown();
			delete m_Direct3D;
			m_Direct3D = 0;
		}

		return;
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
		m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

		// Generate the view matrix based on the camera's position.
		m_Camera->Render();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		auto worldMatrix = DirectX::XMMATRIX{};
		m_Direct3D->GetWorldMatrix(worldMatrix);
		auto viewMatrix = DirectX::XMMATRIX{};
		m_Camera->GetViewMatrix(viewMatrix);
		auto projectionMatrix = DirectX::XMMATRIX{};
		m_Direct3D->GetProjectionMatrix(projectionMatrix);

		// Rotate the world matrix by the rotation value so that the triangle will spin.
		worldMatrix = DirectX::XMMatrixRotationY(rotation);

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Model->Render(m_Direct3D->GetDeviceContext());

		// Render the model using the light shader.
		auto result = m_LightShader->Render(
			m_Direct3D->GetDeviceContext(), 
			m_Model->GetIndexCount(), 
			worldMatrix, 
			viewMatrix, 
			projectionMatrix, 
			m_Model->GetTexture(),
			m_Light->GetDirection(), 
			m_Light->GetDiffuseColor()
		);
		if (not result)
			return false;

		// Present the rendered scene to the screen.
		m_Direct3D->EndScene();

		return true;
	}

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Model = nullptr;
	LightShaderClass* m_LightShader = nullptr;
	LightClass* m_Light = nullptr;
};
