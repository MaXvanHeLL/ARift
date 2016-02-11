#ifndef _EYEWINDOW_H_
#define _EYEWINDOW_H_

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
/// <summary>
/// For debug purposes a window for each eye can be provided. This class is very similar to Bitmap and a merge with it should be considered.
/// <\summary>
class EyeWindow
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	EyeWindow();
	EyeWindow(const EyeWindow&);
	~EyeWindow();

	bool Initialize(ID3D11Device*, int, int, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer *vertexBuffer_, *indexBuffer_;
	int vertexCount_, indexCount_;
	int screenWidth_, screenHeight_;
	int bitmapWidth_, bitmapHeight_;
	int previousPosX_, previousPosY_;
};

#endif
