#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_

#include <d3d11.h>
// #include <d3dx10math.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
// #include <d3dx11async.h>
#include <fstream>

using namespace std;
using namespace DirectX;

class ColorShader
{
	private:
		ID3D11VertexShader* vertexshader_;
		ID3D11PixelShader* pixelshader_;
		ID3D11InputLayout* layout_;
		ID3D11Buffer* matrixbuffer_;

		struct MatrixBufferType
		{
			XMFLOAT4X4 world;
			XMFLOAT4X4 view;
			XMFLOAT4X4 projection;
		};

	public:
		ColorShader();
		ColorShader(const ColorShader&);
		~ColorShader();

		bool Initialize(ID3D11Device*, HWND);
		void Shutdown();
		bool Render(ID3D11DeviceContext*, int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4);

	private:
		bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
		void ShutdownShader();
		void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

		bool SetShaderParameters(ID3D11DeviceContext*, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4);
		void RenderShader(ID3D11DeviceContext*, int);
};

#endif