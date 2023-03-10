#include "pch.h"
#include "Renderer.h"
#include "Effect.h"
#include "Utils.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}		
		  
		m_pCamera = new Camera{ float(m_Width) / float(m_Height), 45.f, {0.f, 0.f, -50.f} };

		std::vector<Vertex_PosCol> vertices {};
		std::vector<uint32_t> indices {};
		Utils::ParseOBJ("./Resources/vehicle.obj", vertices, indices);

		CompleteEffect* completeEffect = new CompleteEffect(m_pDevice, L"./Resources/PosCol3D.fx");
		m_pMesh = new Mesh{ m_pDevice, vertices, indices, completeEffect };
			
		m_pTexture = Texture::LoadFromFile("./Resources/vehicle_diffuse.png", m_pDevice);
		m_pNormal = Texture::LoadFromFile("./Resources/vehicle_normal.png", m_pDevice);
		m_pSpecular = Texture::LoadFromFile("./Resources/vehicle_specular.png", m_pDevice);
		m_pGlossiness = Texture::LoadFromFile("./Resources/vehicle_gloss.png", m_pDevice);

		m_pMesh->SetMaps(m_pTexture, m_pNormal, m_pSpecular, m_pGlossiness);
		
		std::vector<Vertex_PosCol> verticesFX{};
		std::vector<uint32_t> indicesFX{};
		Utils::ParseOBJ("./Resources/fireFX.obj", verticesFX, indicesFX);

		FlatEffect* flatEffect = new FlatEffect(m_pDevice, L"./Resources/PosCol3DFX.fx");
		m_pFX = new Mesh{ m_pDevice, verticesFX, indicesFX,  flatEffect };

		m_pFXTexture = Texture::LoadFromFile("./Resources/fireFX_diffuse.png", m_pDevice);
		m_pFX->SetMaps(m_pFXTexture);

		//m_pDeviceContext->GenerateMips(m_pTexture->GetResourceView());
	}

	Renderer::~Renderer()
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = nullptr;

		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;

		m_pDevice->Release();
		m_pDevice = nullptr;


		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
			m_pDeviceContext = nullptr;
		}

		m_pSwapChain->Release();
		m_pSwapChain = nullptr;

		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;

		m_pRenderTargetBuffer->Release();
		m_pRenderTargetBuffer = nullptr;		

		delete m_pCamera;
		m_pCamera = nullptr;		

		delete m_pMesh;
		m_pMesh = nullptr;

		delete m_pTexture;
		m_pTexture = nullptr;

		delete m_pNormal;
		m_pNormal = nullptr;

		delete m_pSpecular;
		m_pSpecular = nullptr;

		delete m_pGlossiness;
		m_pGlossiness = nullptr;
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);

		Matrix viewProjMatrix{ m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix()};
		Matrix inverseMatrix{ m_pCamera->GetViewMatrix().Inverse() };
		Matrix worldMatrix{ m_pCamera->GetViewMatrix() };
		float matrix[16];
		for (int i{ 0 }; i < 4; ++i)
		{
			for (int j{ 0 }; j < 4; ++j)
			{
				matrix[4 * i + j] = viewProjMatrix[i][j];
			}
		}
		m_pMesh->SetViewProjMatrix(&matrix[0]);
		m_pFX->SetViewProjMatrix(&matrix[0]);
		//m_pMesh->SetInverseMatrix();
	}

	void Renderer::Render()
	{ 
		if (!m_IsInitialized)
			return;

		//1. clear RTV & DSV
		ColorRGB clearColor = ColorRGB{ 0.f, 0.f, 0.3f };;
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. set pipeline + invoke drawcalls (= render)		
		m_pMesh->Render(m_pDeviceContext);
		m_pFX->Render(m_pDeviceContext);

		//3. present backbuffer (swap)
		m_pSwapChain->Present(0, 0);
	}

	void Renderer::LoopSampleState()
	{
		switch (m_pMesh->m_CurrentTechnique)
		{
		case TechniqueType::point:
			m_pMesh->m_CurrentTechnique = TechniqueType::linear;
			m_pFX->m_CurrentTechnique = TechniqueType::linear;
			break;
		case TechniqueType::linear:
			m_pMesh->m_CurrentTechnique = TechniqueType::anisotropic;
			m_pFX->m_CurrentTechnique = TechniqueType::anisotropic;
			break;
		case TechniqueType::anisotropic:
			m_pMesh->m_CurrentTechnique = TechniqueType::point;
			m_pFX->m_CurrentTechnique = TechniqueType::point;
			break;
		}
	}

	HRESULT Renderer::InitializeDirectX()
	{
		//1. create device and device context
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;

	#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 
			&featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
			return result;

		//create DXGI factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));

		if (FAILED(result))
			return result;
		
		//2. create swapchain		
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		//get the handle (HWND) from the SDL Backbuffer
		SDL_SysWMinfo sysWMinfo{};
		SDL_VERSION(&sysWMinfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMinfo);
		swapChainDesc.OutputWindow = sysWMinfo.info.win.window;

		//create swapchain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		//3. create DepthStencil (DS) &  DepthStencilView (DSV)
		//resource	
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//view 
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		//4. create RenderTarget (RT) & RenderTargetViw (RTV)
		//resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//view
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;
	
		//5. bind RTV & DSV to Output Merger Stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		//6. set the viewport (where the content of the backbuffer will be rendered on the screen
		//transforms the NDC to correct screen space
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		pDxgiFactory->Release();

		return S_OK;
	}
}
