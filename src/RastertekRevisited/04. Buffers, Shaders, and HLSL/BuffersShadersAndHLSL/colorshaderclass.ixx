////////////////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.h
////////////////////////////////////////////////////////////////////////////////

export module demo:colorshaderclass;
import std;
import win32;

////////////////////////////////////////////////////////////////////////////////
// Class name: ColorShaderClass
////////////////////////////////////////////////////////////////////////////////
class ColorShaderClass
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public:
	auto Initialize(ID3D11Device* device, HWND hwnd) -> bool
	{
		auto vsFilename = std::wstring{L"color.vs.hlsl"};
		auto psFilename = std::wstring{L"color.ps.hlsl"};
		// Initialize the vertex and pixel shaders.
		if (!InitializeShader(device, hwnd, vsFilename, psFilename))
			return false;

		return true;
	}

	void Shutdown()
	{
		// Shutdown the vertex and pixel shaders as well as the related objects.
		ShutdownShader();
	}

	auto Render(
		ID3D11DeviceContext* deviceContext, 
		int indexCount, 
		DirectX::XMMATRIX worldMatrix, 
		DirectX::XMMATRIX viewMatrix, 
		DirectX::XMMATRIX projectionMatrix
	) -> bool
	{
		// Set the shader parameters that it will use for rendering.
		if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
		{
			return false;
		}

		// Now render the prepared buffers with the shader.
		RenderShader(deviceContext, indexCount);

		return true;
	}

private:
	auto InitializeShader(
		ID3D11Device* device, 
		HWND hwnd, 
		const std::wstring& vsFilename, 
		const std::wstring& psFilename
	) -> bool
	{
		auto errorMessage = static_cast<ID3DBlob*>(nullptr);
		auto vertexShaderBuffer = static_cast<ID3DBlob*>(nullptr);
		// Compile the vertex shader code.
		auto result = D3DCompileFromFile(
			vsFilename.c_str(), 
			nullptr, 
			nullptr, 
			"ColorVertexShader", 
			"vs_5_0", 
			D3D10ShaderEnableStrictness,
			0,
			&vertexShaderBuffer, 
			&errorMessage
		);
		if (Failed(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, const_cast<wchar_t*>(vsFilename.data()));
			}
			// If there was  nothing in the error message then it simply could not find the shader file itself.
			else
			{
				MessageBoxW(hwnd, vsFilename.c_str(), L"Missing Shader File", MB::Ok);
			}

			return false;
		}

		// Compile the pixel shader code.
		auto pixelShaderBuffer = static_cast<ID3DBlob*>(nullptr);
		result = D3DCompileFromFile(
			psFilename.c_str(), 
			nullptr, 
			nullptr, 
			"ColorPixelShader", 
			"ps_5_0", 
			D3D10ShaderEnableStrictness, 
			0,
			&pixelShaderBuffer, 
			&errorMessage
		);
		if (Failed(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, const_cast<wchar_t*>(psFilename.data()));
			}
			// If there was nothing in the error message then it simply could not find the file itself.
			else
			{
				MessageBoxW(hwnd, psFilename.c_str(), L"Missing Shader File", MB::Ok);
			}

			return false;
		}


		// Create the vertex shader from the buffer.
		result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
		if (Failed(result))
		{
			return false;
		}

		// Create the pixel shader from the buffer.
		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
		if (Failed(result))
		{
			return false;
		}


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
				.InstanceDataStepRate = 0
			},
			D3D11_INPUT_ELEMENT_DESC{
				.SemanticName = "COLOR",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = D3D11AppendAlignedElement,
				.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0
			}
		};

		// Get a count of the elements in the layout.
		unsigned int numElements = static_cast<unsigned int>(polygonLayout.size());

		// Create the vertex input layout.
		result = device->CreateInputLayout(polygonLayout.data(), numElements, vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), &m_layout);
		if (Failed(result))
		{
			return false;
		}

		// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
		vertexShaderBuffer->Release();
		vertexShaderBuffer = 0;

		pixelShaderBuffer->Release();
		pixelShaderBuffer = 0;


		// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
		auto matrixBufferDesc = D3D11_BUFFER_DESC{
			.ByteWidth = sizeof(MatrixBufferType),
			.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0,
			.StructureByteStride = 0
		};

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
		if (Failed(result))
		{
			return false;
		}

		return true;
	}

	void ShutdownShader()
	{
		// Release the matrix constant buffer.
		if (m_matrixBuffer)
		{
			m_matrixBuffer->Release();
			m_matrixBuffer = 0;
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

	void OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, wchar_t* shaderFilename)
	{
		char* compileErrors;
		unsigned long long bufferSize, i;
		std::ofstream fout;


		// Get a pointer to the error message text buffer.
		compileErrors = (char*)(errorMessage->GetBufferPointer());

		// Get the length of the message.
		bufferSize = errorMessage->GetBufferSize();

		// Open a file to write the error message to.
		fout.open("shader-error.txt");

		// Write out the error message.
		for (i = 0; i < bufferSize; i++)
		{
			fout << compileErrors[i];
		}

		// Close the file.
		fout.close();

		// Release the error message.
		errorMessage->Release();
		errorMessage = 0;

		// Pop a message up on the screen to notify the user to check the text file for compile errors.
		MessageBoxW(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB::Ok);
	}

	auto SetShaderParameters(
		ID3D11DeviceContext* deviceContext, 
		DirectX::XMMATRIX worldMatrix, 
		DirectX::XMMATRIX viewMatrix, 
		DirectX::XMMATRIX projectionMatrix
	) -> bool
	{
		// Make sure to transpose matrices before sending them into the shader, this is a requirement for DirectX 11.

		// Transpose the matrices to prepare them for the shader.
		worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
		viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
		projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

		// Lock the m_matrixBuffer, set the new matrices inside it, and then unlock it.

		// Lock the constant buffer so it can be written to.
		auto mappedResource = D3D11_MAPPED_SUBRESOURCE{};
		auto result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (Failed(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		auto dataPtr = (MatrixBufferType*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->world = worldMatrix;
		dataPtr->view = viewMatrix;
		dataPtr->projection = projectionMatrix;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_matrixBuffer, 0);

		// Set the position of the constant buffer in the vertex shader.
		auto bufferNumber = 0u;
		// Finanly set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

		return true;
	}


	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
	{
		// Set the vertex input layout.
		deviceContext->IASetInputLayout(m_layout);

		// Set the vertex and pixel shaders that will be used to render this triangle.
		deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
		deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

		// Render the triangle.
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}


private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	ID3D11Buffer* m_matrixBuffer = nullptr;
};
