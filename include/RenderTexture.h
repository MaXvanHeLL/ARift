#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include <d3d11.h>
/// <summary>
/// Initializes, updates and destroys a render target that can be accessed as texture. All rendering is done to such a RenderTexture whose texture is then passed on to the Oculus SDK.
/// <\summary>
class RenderTexture
{
	public:
		RenderTexture();
		RenderTexture(const RenderTexture&);
		~RenderTexture();

		bool Initialize(ID3D11Device*, int, int);
		void Shutdown();

		void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
		void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
		ID3D11ShaderResourceView* GetShaderResourceView();

		ID3D11Texture2D* renderTargetTexture_;
	private:

		ID3D11ShaderResourceView* shaderResourceView_;
		ID3D11RenderTargetView* renderTargetView_;
};

#endif
