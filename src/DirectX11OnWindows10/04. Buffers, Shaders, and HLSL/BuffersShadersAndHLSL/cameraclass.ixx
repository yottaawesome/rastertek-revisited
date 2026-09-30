export module demo:cameraclass;
import std;
import win32;

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	void SetPosition(float x, float y, float z)
	{
		m_positionX = x;
		m_positionY = y;
		m_positionZ = z;
	}

	void SetRotation(float x, float y, float z)
	{
		m_rotationX = x;
		m_rotationY = y;
		m_rotationZ = z;
	}

	auto GetPosition() -> DirectX::XMFLOAT3
	{
		return DirectX::XMFLOAT3(m_positionX, m_positionY, m_positionZ);
	}

	auto GetRotation() -> DirectX::XMFLOAT3
	{
		return DirectX::XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
	}

	void Render()
	{
		// Load it into a XMVECTOR structure.
		auto upVector = DirectX::XMVECTOR{ 0, 1, 0, 0 };

		// Load it into a XMVECTOR structure.
		// Setup the position of the camera in the world.
		auto positionVector = DirectX::XMVECTOR{  m_positionX, m_positionY, m_positionZ, 0 };

		// Setup where the camera is looking by default.
		auto lookAtVector = DirectX::XMVECTOR{ 0, 0, 1, 0 };

		// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
		auto pitch = m_rotationX * 0.0174532925f;
		auto yaw = m_rotationY * 0.0174532925f;
		auto roll = m_rotationZ * 0.0174532925f;

		// Create the rotation matrix from the yaw, pitch, and roll values.
		auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

		// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
		lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
		upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

		// Translate the rotated camera position to the location of the viewer.
		lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

		// Finally create the view matrix from the three updated vectors.
		m_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
	}


	void GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
	{
		viewMatrix = m_viewMatrix;
	}

private:
	float m_positionX = 0.0f; 
	float m_positionY = 0.0f; 
	float m_positionZ = 0.0f;
	float m_rotationX = 0.0f; 
	float m_rotationY = 0.0f; 
	float m_rotationZ = 0.0f;
	DirectX::XMMATRIX m_viewMatrix;
};
