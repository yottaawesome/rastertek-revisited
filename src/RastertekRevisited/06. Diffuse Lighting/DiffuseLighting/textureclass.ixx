////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
export module demo:textureclass;
import std;
import std.compat;
import win32;

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	auto Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) -> bool
	{
		// Load the targa image data into memory.
		bool result = LoadTarga32Bit(filename);
		if (!result)
			return false;

		// Setup the description of the texture.
		auto textureDesc = D3D11_TEXTURE2D_DESC{
			.Width = static_cast<unsigned int>(m_width),
			.Height = static_cast<unsigned int>(m_height),
			.MipLevels = 0,
			.ArraySize = 1,
			.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
			.SampleDesc = { .Count = 1, .Quality = 0 },
			.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_FLAG{ D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET },
			.CPUAccessFlags = 0,
			.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_GENERATE_MIPS
		};

		// Create the empty texture.
		auto hResult = device->CreateTexture2D(&textureDesc, nullptr, &m_texture);
		if (Failed(hResult))
		{
			return false;
		}

		// Set the row pitch of the targa image data.
		auto rowPitch = static_cast<unsigned int>((m_width * 4) * sizeof(unsigned char));

		// Copy the targa image data into the texture.
		deviceContext->UpdateSubresource(m_texture, 0, nullptr, m_targaData, rowPitch, 0);

		// Setup the shader resource view description.
		auto srvDesc = D3D11_SHADER_RESOURCE_VIEW_DESC{
			.Format = textureDesc.Format,
			.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D,
			.Texture2D = {
				.MostDetailedMip = 0, 
				.MipLevels = std::numeric_limits<unsigned>::max()
			}
		};

		// Create the shader resource view for the texture.
		hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
		if (Failed(hResult))
		{
			return false;
		}

		// Generate mipmaps for this texture.
		deviceContext->GenerateMips(m_textureView);

		// Release the targa image data now that the image data has been loaded into the texture.
		delete[] m_targaData;
		m_targaData = 0;

		return true;
	}

	void Shutdown()
	{
		// Release the texture view resource.
		if (m_textureView)
		{
			m_textureView->Release();
			m_textureView = 0;
		}

		// Release the texture.
		if (m_texture)
		{
			m_texture->Release();
			m_texture = 0;
		}

		// Release the targa data.
		if (m_targaData)
		{
			delete[] m_targaData;
			m_targaData = 0;
		}

		return;
	}

	auto GetTexture() -> ID3D11ShaderResourceView*
	{
		return m_textureView;
	}

	auto GetWidth() -> int
	{
		return m_width;
	}

	auto GetHeight() -> int
	{
		return m_height;
	}

private:
	auto LoadTarga32Bit(char* filename) -> bool
	{
		// Open the targa file for reading in binary.
		auto filePtr = static_cast<FILE*>(nullptr);
		auto error = fopen_s(&filePtr, filename, "rb");
		if (error != 0)
		{
			return false;
		}

		// Read in the file header.
		TargaHeader targaFileHeader;
		auto count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
		if (count != 1)
		{
			return false;
		}

		// Get the important information from the header.
		m_height = (int)targaFileHeader.height;
		m_width = (int)targaFileHeader.width;
		auto bpp = (int)targaFileHeader.bpp;

		// Check that it is 32 bit and not 24 bit.
		if (bpp != 32)
		{
			return false;
		}

		// Calculate the size of the 32 bit image data.
		auto imageSize = m_width * m_height * 4;

		// Allocate memory for the targa image data.
		auto targaImage = new unsigned char[imageSize];

		// Read in the targa image data.
		count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
		if (count != imageSize)
		{
			return false;
		}

		// Close the file.
		error = fclose(filePtr);
		if (error != 0)
		{
			return false;
		}

		// Allocate memory for the targa destination data.
		m_targaData = new unsigned char[imageSize];

		// Initialize the index into the targa destination data array.
		auto index = 0;

		// Initialize the index into the targa image data.
		auto k = (m_width * m_height * 4) - (m_width * 4);

		// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
		for (auto j = 0; j < m_height; j++)
		{
			for (auto i = 0; i < m_width; i++)
			{
				m_targaData[index + 0] = targaImage[k + 2];  // Red.
				m_targaData[index + 1] = targaImage[k + 1];  // Green.
				m_targaData[index + 2] = targaImage[k + 0];  // Blue
				m_targaData[index + 3] = targaImage[k + 3];  // Alpha

				// Increment the indexes into the targa data.
				k += 4;
				index += 4;
			}

			// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
			k -= (m_width * 8);
		}

		// Release the targa image data now that it was copied into the destination array.
		delete[] targaImage;
		targaImage = 0;

		return true;
	}

private:
	unsigned char* m_targaData = nullptr;
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11ShaderResourceView* m_textureView = nullptr;
	int m_width = 0;
	int m_height = 0;
};
