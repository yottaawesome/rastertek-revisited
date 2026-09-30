////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
export module demo:lightshaderclass;
import std;
import win32;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT3 lightDirection;
		float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};

public:
	auto Initialize(ID3D11Device* device, HWND hwnd) -> bool
	{
		// Set the filename of the vertex shader.
		auto vsFilename = std::wstring{L"light.vs.hlsl"};
		// Set the filename of the pixel shader.
		auto psFilename = std::wstring{L"light.ps.hlsl"};

		// Initialize the vertex and pixel shaders.
		if (not InitializeShader(device, hwnd, vsFilename.data(), psFilename.data()))
			return false;

		return true;
	}

	auto Shutdown() -> void
	{
		// Shutdown the vertex and pixel shaders as well as the related objects.
		ShutdownShader();
	}

	auto Render(
		ID3D11DeviceContext* deviceContext,
		int indexCount,
		DirectX::XMMATRIX worldMatrix,
		DirectX::XMMATRIX viewMatrix,
		DirectX::XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture,
		DirectX::XMFLOAT3 lightDirection,
		DirectX::XMFLOAT4 diffuseColor
	) -> bool
	{
		// Set the shader parameters that it will use for rendering.
		auto result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
		if (not result)
			return false;

		// Now render the prepared buffers with the shader.
		RenderShader(deviceContext, indexCount);

		return true;
	}

private:
	auto InitializeShader(ID3D11Device* device, HWND hwnd, const std::wstring& vsFilename, const std::wstring& psFilename) -> bool
	{
		// Initialize the pointers this function will use to null.

		// Compile the vertex shader code.
		auto errorMessage = static_cast<ID3DBlob*>(nullptr);
		auto vertexShaderBuffer = static_cast<ID3DBlob*>(nullptr);
		auto result = D3DCompileFromFile(vsFilename.c_str(), nullptr, nullptr, "LightVertexShader", "vs_5_0", D3D10ShaderEnableStrictness, 0, &vertexShaderBuffer, &errorMessage);
		if (Failed(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
				OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else
				MessageBoxW(hwnd, vsFilename.c_str(), L"Missing Shader File", MB::Ok);

			return false;
		}

		// Compile the pixel shader code.
		auto pixelShaderBuffer = static_cast<ID3DBlob*>(nullptr);
		result = D3DCompileFromFile(psFilename.c_str(), nullptr, nullptr, "LightPixelShader", "ps_5_0", D3D10ShaderEnableStrictness, 0, &pixelShaderBuffer, &errorMessage);
		if (Failed(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
				OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
			// If there was nothing in the error message then it simply could not find the file itself.
			else
				MessageBoxW(hwnd, psFilename.c_str(), L"Missing Shader File", MB::Ok);

			return false;
		}

		// Create the vertex shader from the buffer.
		result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
		if (Failed(result))
			return false;

		// Create the pixel shader from the buffer.
		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
		if (Failed(result))
			return false;

		// Create the vertex input layout description.
		// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
		auto polygonLayout = std::array{
			D3D11_INPUT_ELEMENT_DESC{
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = 0,
				.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			D3D11_INPUT_ELEMENT_DESC{
				.SemanticName = "TEXCOORD",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = D3D11AppendAlignedElement,
				.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			D3D11_INPUT_ELEMENT_DESC{
				.SemanticName = "NORMAL",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = D3D11AppendAlignedElement,
				.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			}
		};

		// Create the vertex input layout.
		result = device->CreateInputLayout(
			polygonLayout.data(), 
			static_cast<unsigned int>(polygonLayout.size()), 
			vertexShaderBuffer->GetBufferPointer(), 
			vertexShaderBuffer->GetBufferSize(),
			&m_layout
		);
		if (Failed(result))
			return false;

		// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
		vertexShaderBuffer->Release();
		vertexShaderBuffer = 0;

		pixelShaderBuffer->Release();
		pixelShaderBuffer = 0;

		// Create a texture sampler state description.
		auto samplerDesc = D3D11_SAMPLER_DESC{
			.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
			.MipLODBias = 0.0f,
			.MaxAnisotropy = 1,
			.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS,
			.BorderColor = {0, 0, 0, 0},
			.MinLOD = 0,
			.MaxLOD = std::numeric_limits<float>::max()
		};

		// Create the texture sampler state.
		result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
		if (Failed(result))
			return false;

		// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
		auto matrixBufferDesc = D3D11_BUFFER_DESC{
			.ByteWidth = sizeof(MatrixBufferType),
			.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0,
			.StructureByteStride = 0
		};

		matrixBufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
		if (Failed(result))
			return false;

		// Setup the description of the light dynamic constant buffer that is in the pixel shader.
		// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
		auto lightBufferDesc = D3D11_BUFFER_DESC{
			.ByteWidth = sizeof(LightBufferType),
			.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0,
			.StructureByteStride = 0
		};
		lightBufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
		if (Failed(result))
			return false;

		return true;
	}

	void ShutdownShader()
	{
		// Release the light constant buffer.
		if (m_lightBuffer)
		{
			m_lightBuffer->Release();
			m_lightBuffer = 0;
		}

		// Release the matrix constant buffer.
		if (m_matrixBuffer)
		{
			m_matrixBuffer->Release();
			m_matrixBuffer = 0;
		}

		// Release the sampler state.
		if (m_sampleState)
		{
			m_sampleState->Release();
			m_sampleState = 0;
		}

		// Release the layout.
		if (m_layout)
		{
			m_layout->Release();
			m_layout = 0;
		}

		// Release the pixel shader.
		if (m_pixelShader)
		{
			m_pixelShader->Release();
			m_pixelShader = 0;
		}

		// Release the vertex shader.
		if (m_vertexShader)
		{
			m_vertexShader->Release();
			m_vertexShader = 0;
		}
	}

	void OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, const std::wstring& shaderFilename)
	{
		// Get a pointer to the error message text buffer.
		auto compileErrors = (char*)(errorMessage->GetBufferPointer());

		// Get the length of the message.
		auto bufferSize = errorMessage->GetBufferSize();

		// Open a file to write the error message to.
		auto fout = std::ofstream("shader-error.txt");

		// Write out the error message.
		for (auto i = 0ull; i < bufferSize; i++)
			fout << compileErrors[i];

		// Close the file.
		fout.close();

		// Release the error message.
		errorMessage->Release();
		errorMessage = 0;

		// Pop a message up on the screen to notify the user to check the text file for compile errors.
		MessageBoxW(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename.c_str(), MB::Ok);
	}

	auto SetShaderParameters(
		ID3D11DeviceContext* deviceContext,
		DirectX::XMMATRIX worldMatrix,
		DirectX::XMMATRIX viewMatrix,
		DirectX::XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture,
		DirectX::XMFLOAT3 lightDirection,
		DirectX::XMFLOAT4 diffuseColor
	) -> bool
	{
		// Transpose the matrices to prepare them for the shader.
		worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
		viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
		projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

		// Lock the constant buffer so it can be written to.
		auto mappedResource = D3D11_MAPPED_SUBRESOURCE{};
		auto result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (Failed(result))
			return false;

		// Get a pointer to the data in the constant buffer.
		auto dataPtr = reinterpret_cast<MatrixBufferType*>(mappedResource.pData);

		// Copy the matrices into the constant buffer.
		dataPtr->world = worldMatrix;
		dataPtr->view = viewMatrix;
		dataPtr->projection = projectionMatrix;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_matrixBuffer, 0);

		// Set the position of the constant buffer in the vertex shader.
		auto bufferNumber = 0u;

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

		// Set shader texture resource in the pixel shader.
		deviceContext->PSSetShaderResources(0, 1, &texture);

		// Lock the light constant buffer so it can be written to.
		result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (Failed(result))
			return false;

		// Get a pointer to the data in the constant buffer.
		auto dataPtr2 = reinterpret_cast<LightBufferType*>(mappedResource.pData);

		// Copy the lighting variables into the constant buffer.
		dataPtr2->diffuseColor = diffuseColor;
		dataPtr2->lightDirection = lightDirection;
		dataPtr2->padding = 0.0f;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_lightBuffer, 0);

		// Set the position of the light constant buffer in the pixel shader.
		bufferNumber = 0;

		// Finally set the light constant buffer in the pixel shader with the updated values.
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

		return true;
	}

	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
	{
		// Set the vertex input layout.
		deviceContext->IASetInputLayout(m_layout);

		// Set the vertex and pixel shaders that will be used to render this triangle.
		deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
		deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

		// Set the sampler state in the pixel shader.
		deviceContext->PSSetSamplers(0, 1, &m_sampleState);

		// Render the triangle.
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}

private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;
	ID3D11Buffer* m_matrixBuffer = nullptr;
	ID3D11Buffer* m_lightBuffer = nullptr;
};
