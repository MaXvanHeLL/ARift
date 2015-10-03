#ifndef _TEXTURE_H_
#define _TEXTURE_H_

# include <d3d11.h>
# include <DirectXMath.h>
// Debug
# include <DDSTextureLoader.h>
// Release
// # include "../include/DDSTextureLoader.h"
class IDSuEyeInputHandler;
using namespace DirectX;

class Texture
{
	private:
		ID3D11ShaderResourceView* shaderResource_;
		ID3D11Texture2D* cameraTexture_;

	public:
		Texture();
		Texture(const Texture&);
		~Texture();

		bool Initialize(ID3D11Device*, WCHAR*);
    bool InitCameraStream(ID3D11Device*, IDSuEyeInputHandler*);
    bool Update(ID3D11DeviceContext*, IDSuEyeInputHandler*,int);

		void Shutdown();

		ID3D11ShaderResourceView* GetTexture();
};

#endif