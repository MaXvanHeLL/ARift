////////////////////////////////////////////////////////////////////////////////
// Filename: BitMap.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BitMap_H_
#define _BitMap_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "Texture.h"

using namespace DirectX;

class Texture;

class BitMap
{
private:
	ID3D11Buffer* vertexbuffer_;
	ID3D11Buffer* indexbuffer_;
	int vertexcount_, indexcount_;
	Texture* texture_;

	int screenwidth_, screenheight_;
	int bitmapwidth_, bitmapheight_;
	int previousposX_, previousposY_;

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	BitMap();
	BitMap(const BitMap&);
	~BitMap();

	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture(); 
	int GetPositionX();
	int GetPositionY();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();
};

#endif
