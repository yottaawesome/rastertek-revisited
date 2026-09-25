export module demo:d3dclass;
import win32;

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:

    bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
	{


		// Store the vsync setting.
		m_vsync_enabled = vsync;

		// Create a DirectX graphics interface factory.
		IDXGIFactory* factory;
		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (Failed(result))
			return false;

		// Use the factory to create an adapter for the primary graphics interface (video card).
		IDXGIAdapter* adapter;
		result = factory->EnumAdapters(0, &adapter);
		if (Failed(result))
			return false;

		// Enumerate the primary adapter output (monitor).
		IDXGIOutput* adapterOutput;
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (Failed(result))
			return false;

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		unsigned int numModes;
		result = adapterOutput->GetDisplayModeList(
			DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, 
			DxgiEnumModes::Interlaced, 
			&numModes, 
			nullptr
		);
		if (Failed(result))
			return false;

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
			return false;

		// Now fill the display mode list structures.
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, DxgiEnumModes::Interlaced, &numModes, displayModeList);
		if (Failed(result))
			return false;

		// Now go through all the display modes and find the one that matches the screen width and height.
		// When a match is found store the numerator and denominator of the refresh rate for that monitor.
		unsigned int numerator;
		unsigned int denominator;
		for (unsigned int i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int)screenWidth)
			{
				if (displayModeList[i].Height == (unsigned int)screenHeight)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		// Get the adapter (video card) description.
		DXGI_ADAPTER_DESC adapterDesc;
		result = adapter->GetDesc(&adapterDesc);
		if (Failed(result))
			return false;

		// Store the dedicated video card memory in megabytes.
		m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// Convert the name of the video card to a character array and store it.
		unsigned long long stringLength;
		int error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
		if (error != 0)
			return false;

		// Release the display mode list.
		delete[] displayModeList;
		displayModeList = 0;

		// Release the adapter output.
		adapterOutput->Release();
		adapterOutput = 0;

		// Release the adapter.
		adapter->Release();
		adapter = 0;

		// Release the factory.
		factory->Release();
		factory = 0;

		// Initialize the swap chain description.
		auto swapChainDesc = DXGI_SWAP_CHAIN_DESC{
			// Set the width and height of the back buffer.
			.BufferDesc = {
				.Width = static_cast<UINT>(screenWidth),
				.Height = static_cast<UINT>(screenHeight),
				// Set the refresh rate of the back buffer.
				.RefreshRate = 
					m_vsync_enabled 
						? DXGI_RATIONAL{.Numerator = numerator, .Denominator = denominator } 
						: DXGI_RATIONAL{.Numerator = 0, .Denominator = 1 },
				// Set regular 32-bit surface for the back buffer.
				.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
				// Set the scan line ordering and scaling to unspecified.
				.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
				.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED
			},
			// Turn multisampling off.
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			// Set the usage of the back buffer.
			.BufferUsage = DxgiUsage::RenderTargetOutput,
			// Set to a single back buffer.
			.BufferCount = 1,
			// Set the handle for the window to render to.
			.OutputWindow = hwnd,
			// Set to full screen or windowed mode.
			.Windowed = not fullscreen,
			// Discard the back buffer contents after presenting.
			.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD,
			// Don't set the advanced flags.
			.Flags = 0
		};

		// Set the feature level to DirectX 11.
		auto featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

		// Create the swap chain, Direct3D device, and Direct3D device context.
		result = D3D11CreateDeviceAndSwapChain(
			nullptr, 
			D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, 
			nullptr, 
			0, 
			&featureLevel, 
			1,
			D3d11SdkVersion, 
			&swapChainDesc, 
			&m_swapChain, 
			&m_device, 
			nullptr, 
			&m_deviceContext
		);
		if (Failed(result))
			return false;

		// Get the pointer to the back buffer.
		auto backBufferPtr = static_cast<ID3D11Texture2D*>(nullptr);
		result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
		if (Failed(result))
			return false;

		// Create the render target view with the back buffer pointer.
		result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
		if (Failed(result))
			return false;

		// Release pointer to the back buffer as we no longer need it.
		backBufferPtr->Release();
		backBufferPtr = nullptr;

		// Initialize the description of the depth buffer.
		// Set up the description of the depth buffer.
		auto depthBufferDesc = D3D11_TEXTURE2D_DESC{
			.Width = static_cast<UINT>(screenWidth),
			.Height = static_cast<UINT>(screenHeight),
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL,
			.CPUAccessFlags = 0,
			.MiscFlags = 0
		};

		// Create the texture for the depth buffer using the filled out description.
		result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
		if (Failed(result))
			return false;

		// Initialize the description of the stencil state.
		auto depthStencilDesc = D3D11_DEPTH_STENCIL_DESC{
			// Set up the description of the stencil state.
			.DepthEnable = true,
			.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL,
			.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS,
			.StencilEnable = true,
			.StencilReadMask = 0xFF,
			.StencilWriteMask = 0xFF,
			// Stencil operations if pixel is front-facing.
			.FrontFace = {
				.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR,
				.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP,
				.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS,
			},
			// Stencil operations if pixel is back-facing.
			.BackFace = {
				.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP,
				.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_DECR,
				.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP,
				.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS,
			},
		};

		// Create the depth stencil state.
		result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (Failed(result))
			return false;

		// Set the depth stencil state.
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

		// Initialize the depth stencil view.
		// Set up the depth stencil view description.
		auto depthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC{
			.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
			.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D,
			.Texture2D = {
				.MipSlice = 0
			}
		};
		
		// Create the depth stencil view.
		result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (Failed(result))
			return false;

		// Bind the render target view and depth stencil buffer to the output render pipeline.
		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

		// Setup the raster description which will determine how and what polygons will be drawn.
		auto rasterDesc = D3D11_RASTERIZER_DESC{
			.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID,
			.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK,
			.FrontCounterClockwise = false,
			.DepthBias = 0,
			.DepthBiasClamp = 0.0f,
			.SlopeScaledDepthBias = 0.0f,
			.DepthClipEnable = true,
			.ScissorEnable = false,
			.MultisampleEnable = false,
			.AntialiasedLineEnable = false,
		};

		// Create the rasterizer state from the description we just filled out.
		result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
		if (Failed(result))
			return false;

		// Now set the rasterizer state.
		m_deviceContext->RSSetState(m_rasterState);

		// Setup the viewport for rendering.
		m_viewport.Width = (float)screenWidth;
		m_viewport.Height = (float)screenHeight;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;

		// Create the viewport.
		m_deviceContext->RSSetViewports(1, &m_viewport);

		// Setup the projection matrix.
		float fieldOfView = 3.141592654f / 4.0f;
		float screenAspect = (float)screenWidth / (float)screenHeight;

		// Create the projection matrix for 3D rendering.
		m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

		// Initialize the world matrix to the identity matrix.
		m_worldMatrix = DirectX::XMMatrixIdentity();

		// Create an orthographic projection matrix for 2D rendering.
		m_orthoMatrix = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

		return true;
	}

    void Shutdown()
    {
        // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
        if (m_swapChain)
        {
            m_swapChain->SetFullscreenState(false, nullptr);
        }

        if (m_rasterState)
        {
            m_rasterState->Release();
            m_rasterState = 0;
        }

        if (m_depthStencilView)
        {
            m_depthStencilView->Release();
            m_depthStencilView = 0;
        }

        if (m_depthStencilState)
        {
            m_depthStencilState->Release();
            m_depthStencilState = 0;
        }

        if (m_depthStencilBuffer)
        {
            m_depthStencilBuffer->Release();
            m_depthStencilBuffer = 0;
        }

        if (m_renderTargetView)
        {
            m_renderTargetView->Release();
            m_renderTargetView = 0;
        }

        if (m_deviceContext)
        {
            m_deviceContext->Release();
            m_deviceContext = 0;
        }

        if (m_device)
        {
            m_device->Release();
            m_device = 0;
        }

        if (m_swapChain)
        {
            m_swapChain->Release();
            m_swapChain = 0;
        }

        return;
    }

    void BeginScene(float red, float green, float blue, float alpha)
    {
        float color[4];


        // Setup the color to clear the buffer to.
        color[0] = red;
        color[1] = green;
        color[2] = blue;
        color[3] = alpha;

        // Clear the back buffer.
        m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

        // Clear the depth buffer.
        m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    void EndScene()
    {
        // Present the back buffer to the screen since rendering is complete.
        if (m_vsync_enabled)
        {
            // Lock to screen refresh rate.
            m_swapChain->Present(1, 0);
        }
        else
        {
            // Present as fast as possible.
            m_swapChain->Present(0, 0);
        }
    }

	auto GetDevice() -> ID3D11Device*
    {
        return m_device;
    }

    auto GetDeviceContext() -> ID3D11DeviceContext*
    {
        return m_deviceContext;
    }

    void GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
    {
        projectionMatrix = m_projectionMatrix;
    }
    
    void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
    {
        worldMatrix = m_worldMatrix;
    }

    void GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
    {
        orthoMatrix = m_orthoMatrix;
    }

    void GetVideoCardInfo(char* cardName, int& memory)
    {
        strcpy_s(cardName, 128, m_videoCardDescription);
        memory = m_videoCardMemory;
    }

    void SetBackBufferRenderTarget()
    {
        // Bind the render target view and depth stencil buffer to the output render pipeline.
        m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
    }
    void ResetViewport()
    {
        // Set the viewport.
        m_deviceContext->RSSetViewports(1, &m_viewport);
    }

private:
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    ID3D11Texture2D* m_depthStencilBuffer = nullptr;
    ID3D11DepthStencilState* m_depthStencilState = nullptr;
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
    ID3D11RasterizerState* m_rasterState = nullptr;
    DirectX::XMMATRIX m_projectionMatrix;
    DirectX::XMMATRIX m_worldMatrix;
    DirectX::XMMATRIX m_orthoMatrix;
    D3D11_VIEWPORT m_viewport;
};
