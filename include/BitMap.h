////////////////////////////////////////////////////////////////////////////////
// Filename: BitMap.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BitMap_H_
#define _BitMap_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "Texture.h"
#include "GraphicsAPI.h"
#include "IDSuEyeInputHandler.h"

using namespace DirectX;

class ARiftControl;
class Texture;
/// <summary>
/// To meld virtual and real scene the camera images need to be rendered, this class provides the required functionality. It is a virtual object - a rectangle - that is always placed so that it fills the respective eyes full field of view. The camera images are then rendered using an undistortion shader onto this rectangle as a texture. Finally a orthographic projection is applied to it in GraphicsAPI.
/// <\summary>
class BitMap
{
private:
	ID3D11Buffer* vertexBuffer_;
	ID3D11Buffer* indexBuffer_;
	int vertexCount_, indexCount_;
	Texture* texture_;

	int screenWidth_, screenHeight_;
	int bitmapWidth_, bitmapHeight_;
	int previousPosX_, previousPosY_;
  double scale_ = 0.0f;

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
  bool InitializeCameras(ID3D11Device* device, int screenWidth, int screenHeight, ARiftControl* arift_control,
                         int bitmapWidth = CAMERA_WIDTH, int bitmapHeight = CAMERA_HEIGHT);
	void Shutdown();
  bool Render(ID3D11DeviceContext* deviceContext, ARiftControl* ariftControl, int camId);
  bool Render(ID3D11DeviceContext* deviceContext, ARiftControl* ariftControl, int camId, int positionX, int positionY);

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
	bool LoadCameraStream(ID3D11Device*, ARiftControl*);
	void ReleaseTexture();
};

#endif
