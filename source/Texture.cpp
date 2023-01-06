#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface, ID3D11Device* pDevice) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);

		SDL_FreeSurface(m_pSurface);
		m_pSurface = nullptr;
	}

	Texture::~Texture()
	{
		//delete m_pSurfacePixels;
		//m_pSurfacePixels = nullptr;

		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}		

		m_pResource->Release();
		m_pResource = nullptr;

		m_pSRV->Release();
		m_pSRV = nullptr;
	}

	Texture* Texture::LoadFromFile(const std::string& path, ID3D11Device* pDevice)
	{
		Texture* pTexture{ new Texture(IMG_Load(path.c_str()), pDevice) };
		return pTexture;
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		//Sample the correct texel for the given uv
		
		//uv is between [0,1] -> convert to [0, width] and [0, height]
		int x{ int(uv.x * m_pSurface->w) };
		int y{ int(uv.y * m_pSurface->h) };		
		Uint32 pixelIdx = m_pSurfacePixels[x + (y * m_pSurface->w)];
		SDL_Color sdl_rgb;
		SDL_GetRGB(pixelIdx, m_pSurface->format, &sdl_rgb.r, &sdl_rgb.g, &sdl_rgb.b);
		ColorRGB rgb{ sdl_rgb.r / 255.f, sdl_rgb.g / 255.f, sdl_rgb.b / 255.f };
		return rgb;
	}

	ID3D11ShaderResourceView* Texture::GetResourceView()
	{
		return m_pSRV;
	}
}