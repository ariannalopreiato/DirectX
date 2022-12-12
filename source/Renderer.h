#pragma once
#include "Mesh.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		//display adapter, used to create resources and enumerate capabilities
		ID3D11Device* m_pDevice;

		//setting in which a device is used (set pipeline states, generate rendeing commands) NOT thread safe
		ID3D11DeviceContext* m_pDeviceContext;

		//contains two buffers (screen/front and back) everytime a new frame is displayed they swap place
		IDXGISwapChain* m_pSwapChain;

		//buffer used to mask pixels in an image (if a pixel is drawn or not)
		ID3D11Texture2D* m_pDepthStencilBuffer;

		//access to resource/texture during depth-stencil testing
		ID3D11DepthStencilView* m_pDepthStencilView;

		ID3D11Resource* m_pRenderTargetBuffer;

		//access to resource/texture that is used as render target
		ID3D11RenderTargetView* m_pRenderTargetView;

		//access the resource/texture as a constant buffer
		//ID3D11ShaderResourceView* m_pShaderResourceView;

		//access an unordered resource using a pixel shader or a compute shader
		//ID3D11UnorderedAccessView* m_pUnorderedAccessView;

		Mesh* m_pMesh{};

		//DIRECTX
		HRESULT InitializeDirectX();
		//...
	};
}
