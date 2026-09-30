////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module demo:lightclass;
import std;
import win32;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	void SetDiffuseColor(float red, float green, float blue, float alpha)
	{
		m_diffuseColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	}

	void SetDirection(float x, float y, float z)
	{
		m_direction = DirectX::XMFLOAT3(x, y, z);
	}

	auto GetDiffuseColor() -> DirectX::XMFLOAT4
	{
		return m_diffuseColor;
	}

	auto GetDirection() -> DirectX::XMFLOAT3
	{
		return m_direction;
	}

private:
	DirectX::XMFLOAT4 m_diffuseColor;
	DirectX::XMFLOAT3 m_direction;
};
