////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
export module demo:modelclass;

import std;
import win32;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

public:
	auto Initialize(ID3D11Device* device) -> bool
	{
		// Initialize the vertex and index buffers.
		auto result = InitializeBuffers(device);
		if (not result)
			return false;

		return true;
	}

	void Shutdown()
	{
		// Shutdown the vertex and index buffers.
		ShutdownBuffers();
	}

	void Render(ID3D11DeviceContext* deviceContext)
	{
		// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
		RenderBuffers(deviceContext);
	}

	auto GetIndexCount() -> int
	{
		return m_indexCount;
	}

private:
	auto InitializeBuffers(ID3D11Device* device) -> bool
	{
		// Set the number of vertices in the vertex array.
		m_vertexCount = 3;

		// Set the number of indices in the index array.
		m_indexCount = 3;

		// Create the vertex array.
		auto vertices = std::vector<VertexType>{
			{
				.position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f),  // Bottom left.
				.color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
			},
			{
				.position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  // Top middle.
				.color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
			},
			{
				.position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f),  // Bottom right.
				.color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
			}
		};

		// Create the index array.
		auto indices = std::vector<unsigned long>{
			0, // Bottom left.
			1, // Top middle.
			2 // Bottom right.
		};

		// Set up the description of the static vertex buffer.
		auto vertexBufferDesc = D3D11_BUFFER_DESC{
			.ByteWidth = sizeof(VertexType) * m_vertexCount,
			.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = 0,
			.MiscFlags = 0,
			.StructureByteStride = 0
		};

		// Give the subresource structure a pointer to the vertex data.
		auto vertexData = D3D11_SUBRESOURCE_DATA{
			.pSysMem = vertices.data(),
			.SysMemPitch = 0,
			.SysMemSlicePitch = 0
		};

		// Now create the vertex buffer.
		auto result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (Failed(result))
			return false;

		// Set up the description of the static index buffer.
		auto indexBufferDesc = D3D11_BUFFER_DESC{
			.ByteWidth = sizeof(unsigned long) * m_indexCount,
			.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			.CPUAccessFlags = 0,
			.MiscFlags = 0,
			.StructureByteStride = 0
		};

		// Give the subresource structure a pointer to the index data.
		auto indexData = D3D11_SUBRESOURCE_DATA{
			.pSysMem = indices.data(),
			.SysMemPitch = 0,
			.SysMemSlicePitch = 0
		};

		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (Failed(result))
			return false;

		return true;
	}

	void ShutdownBuffers()
	{
		// Release the index buffer.
		if (m_indexBuffer)
		{
			m_indexBuffer->Release();
			m_indexBuffer = 0;
		}

		// Release the vertex buffer.
		if (m_vertexBuffer)
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = 0;
		}
	}

	void RenderBuffers(ID3D11DeviceContext* deviceContext)
	{
		// Set vertex buffer stride and offset.
		auto stride = static_cast<unsigned int>(sizeof(VertexType));
		auto offset = 0u;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
};
