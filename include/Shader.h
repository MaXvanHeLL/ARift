#ifndef _SHADER_H_
#define _SHADER_H_

#include <d3d11.h>
// #include <d3dx10math.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
// #include <d3dx11async.h>
#include <fstream>

using namespace std;
using namespace DirectX;
/// <summary>
/// Loading, compiling and applying shaders is encapsulated here. The application compiles and loads a lighting pixel and vertex shader for the virtual objects and a undistortion pixel shader from files.
/// <\summary>.
class Shader
{
	private:
		ID3D11VertexShader* vertexshader_;
		ID3D11PixelShader* pixelshader_;
    ID3D11PixelShader* undistortionShader_;
		ID3D11InputLayout* layout_;
		ID3D11Buffer* matrixbuffer_;
    ID3D11Buffer* undistortionBuffer_;
		ID3D11SamplerState* samplestate_;
		ID3D11Buffer* lightBuffer_;

		struct MatrixBufferType
		{
			XMFLOAT4X4 world;
			XMFLOAT4X4 view;
			XMFLOAT4X4 projection;
		};

		struct LightBufferType
		{
			XMFLOAT4 ambientColor;
			XMFLOAT4 diffuseColor;
			XMFLOAT3 lightDirection;
			float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
		};

	public:
    struct UndistortionBuffer
    {
      float Nxc;
      float Nyc;
      float z;

      float p0; 
      float p1; float p2; float p3; 
      float p4; float p5; float p6;
      float p7; float p8; float p9;
      float c; float d; float e;

      float xc;
      float yc;
      float width;
      float height;
    };

		Shader();
		Shader(const Shader&);
		~Shader();

		bool Initialize(ID3D11Device*, HWND);
		void Shutdown();
    bool Render(ID3D11DeviceContext*, int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView*, UndistortionBuffer*, XMFLOAT3, XMFLOAT4, XMFLOAT4);
		bool Render(ID3D11DeviceContext*, int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);

	private:
		bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*,WCHAR*);
		void ShutdownShader();
		void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

		bool SetShaderParameters(ID3D11DeviceContext*, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4);
    bool SetUndistortionParameters(ID3D11DeviceContext*, UndistortionBuffer*);
		void RenderShader(ID3D11DeviceContext*, int, bool);
};

#endif
