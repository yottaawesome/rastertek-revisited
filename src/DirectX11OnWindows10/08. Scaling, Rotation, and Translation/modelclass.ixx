////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.ixx
////////////////////////////////////////////////////////////////////////////////
export module demo:modelclass;
import std;
import win32;
import :textureclass;

class ModelClass
{
private:
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x; 
		float y; 
		float z;
		float tu; 
		float tv;
		float nx; 
		float ny; 
		float nz;
	};

public:
	~ModelClass()
	{
		Shutdown();
	}

	auto Initialize(
		ID3D11Device* device, 
		ID3D11DeviceContext* deviceContext, 
		const std::string& modelFilename, 
		const std::string& textureFilename
	) -> bool
	{
		// Load in the model data.
		if (not LoadModel(modelFilename))
			return false;

		// Initialize the vertex and index buffers.
		if (not InitializeBuffers(device))
			return false;

		// Load the texture for this model.
		if (not LoadTexture(device, deviceContext, textureFilename))
			return false;

		return true;
	}

	void Shutdown()
	{
		// Release the model texture.
		ReleaseTexture();

		// Shutdown the vertex and index buffers.
		ShutdownBuffers();

		// Release the model data.
		ReleaseModel();
	}

	void Render(ID3D11DeviceContext* deviceContext)
	{
		// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
		RenderBuffers(deviceContext);
	}

	auto GetIndexCount() const noexcept -> int
	{
		return m_indexCount;
	}

	auto GetTexture() const noexcept -> ID3D11ShaderResourceView*
	{
		return m_Texture.GetTexture();
	}

private:
	auto InitializeBuffers(ID3D11Device* device) -> bool
	{
		// Create the vertex array.
		auto vertices = std::vector<VertexType>(m_vertexCount);

		// Create the index array.
		auto indices = std::vector<unsigned long>(m_indexCount);

		// Load the vertex array and index array with data.
		for (auto i = 0; i < m_vertexCount; i++)
		{
			vertices[i].position = DirectX::XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
			vertices[i].texture = DirectX::XMFLOAT2(m_model[i].tu, m_model[i].tv);
			vertices[i].normal = DirectX::XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
			indices[i] = i;
		}

		// Set up the description of the static vertex buffer.
		auto vertexBufferDesc = D3D11_BUFFER_DESC{
			.ByteWidth = sizeof(VertexType) * m_vertexCount,
			.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = 0,
			.MiscFlags = 0,
			.StructureByteStride = 0,
		};

		// Give the subresource structure a pointer to the vertex data.
		auto vertexData = D3D11_SUBRESOURCE_DATA{
			.pSysMem = vertices.data(),
			.SysMemPitch = 0,
			.SysMemSlicePitch = 0,
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
			.StructureByteStride = 0,
		};

		// Give the subresource structure a pointer to the index data.
		auto indexData = D3D11_SUBRESOURCE_DATA{
			.pSysMem = indices.data(),
			.SysMemPitch = 0,
			.SysMemSlicePitch = 0,
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
			m_indexBuffer = nullptr;
		}

		// Release the vertex buffer.
		if (m_vertexBuffer)
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = nullptr;
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

	auto LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filename) -> bool
	{
		// Create and initialize the texture object.
		if (not m_Texture.Initialize(device, deviceContext, filename))
			return false;

		return true;
	}

	void ReleaseTexture()
	{
		// Release the texture object.
		m_Texture.Shutdown();
	}

	auto LoadModel(const std::string& filename) -> bool
	{
		// Open the model file.
		auto fin = std::ifstream(filename);

		// If it could not open the file then exit.
		if (fin.fail())
			return false;

		// Read up to the value of vertex count.
		auto input = char{};
		fin.get(input);
		while (input != ':')
			fin.get(input);

		// Read in the vertex count.
		fin >> m_vertexCount;

		// Set the number of indices to be the same as the vertex count.
		m_indexCount = m_vertexCount;

		// Create the model using the vertex count that was read in.
		m_model.resize(m_vertexCount);

		// Read up to the beginning of the data.
		fin.get(input);
		while (input != ':')
			fin.get(input);
		fin.get(input);
		fin.get(input);

		// Read in the vertex data.
		for (auto i = 0; i < m_vertexCount; i++)
		{
			fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
			fin >> m_model[i].tu >> m_model[i].tv;
			fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
		}

		// Close the model file.
		fin.close();

		return true;
	}

	void ReleaseModel()
	{
		m_model.clear();
	}

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	TextureClass m_Texture;
	std::vector<ModelType> m_model;
};
