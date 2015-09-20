#include "../include/BitMap.h"
#include "../include/ARiftControl.h"
#include "../include/IDSuEyeInputHandler.h"
#include <iostream>


BitMap::BitMap()
{
	vertexBuffer_ = 0;
	indexBuffer_ = 0;
	texture_ = 0;
}


BitMap::BitMap(const BitMap& other)
{
}


BitMap::~BitMap()
{
}


bool BitMap::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	// Store the screen size.
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	bitmapWidth_ = bitmapWidth;
	bitmapHeight_ = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	previousPosX_ = -1;
	previousPosY_ = -1;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool BitMap::InitializeCameras(
  ID3D11Device* device,
  int screenWidth,
  int screenHeight,
  ARiftControl* arift_control,
  int bitmapWidth,
  int bitmapHeight)
{
	bool result;

	// Store the screen size.
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	bitmapWidth_ = bitmapWidth;
	bitmapHeight_ = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	previousPosX_ = -1;
	previousPosY_ = -1;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
		return false;

	// Load the texture for this model.
	result = LoadCameraStream(device, arift_control);
	if (!result)
	  return false;

	return true;
}


void BitMap::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

bool BitMap::Render(ID3D11DeviceContext* deviceContext, ARiftControl* ariftControl, int camId)
{
  // center and snap the bitmap on the screen as large as possible
  double bitmapAspect = bitmapWidth_ / ((double) bitmapHeight_);
  int x_offset = 0;
  int y_offset = 0;
  if (bitmapAspect > 1)
  {
    y_offset = (int)((screenHeight_ - screenWidth_ / bitmapAspect) / 2.0);
    scale_ = screenWidth_ / (double)bitmapWidth_;
  }
  else
  {
    x_offset = (int)((screenWidth_ - screenHeight_ * bitmapAspect) / 2.0);
    scale_ = screenHeight_ / (double)bitmapHeight_;
  }
  if ((x_offset != previousPosX_) || (y_offset != previousPosY_))
    std::cout << "bitmapAspect " << bitmapAspect << " (x_offset , y_offset)  ( " << x_offset << " , " << y_offset << " )" << std::endl;
  return Render(deviceContext, ariftControl, camId, x_offset, y_offset);
}

bool BitMap::Render(ID3D11DeviceContext* deviceContext, ARiftControl* arift_control, int cam_id, int positionX, int positionY)
{
	bool result = 0;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}
	
	if (AR_HMD_ENABLED)
	{
		result = texture_->Update(deviceContext, arift_control->camInput_, cam_id);
		if (!result)
		{
			std::cout << "Error: Could not Update Bitmap Object!" << std::endl;
			return false;
		}
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}


int BitMap::GetIndexCount()
{
	return indexCount_;
}


int BitMap::GetPositionX()
{
	return previousPosX_;
}


int BitMap::GetPositionY()
{
	return previousPosY_;
}


ID3D11ShaderResourceView* BitMap::GetTexture()
{
	return texture_->GetTexture();
}


bool BitMap::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Set the number of vertices in the vertex array.
	vertexCount_ = 6;

	// Set the number of indices in the index array.
	indexCount_ = vertexCount_;

	// Create the vertex array.
	vertices = new VertexType[vertexCount_];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount_];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * vertexCount_));

	// Load the index array with data.
	for (i = 0; i < indexCount_; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage =          D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth =      sizeof(VertexType) * vertexCount_;
	vertexBufferDesc.BindFlags =      D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer_);
	if (FAILED(result))
		return false;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage =     D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount_;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer_);
	if (FAILED(result))
		return false;

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void BitMap::ShutdownBuffers()
{
	// Release the index buffer.
	if (indexBuffer_)
	{
		indexBuffer_->Release();
		indexBuffer_ = 0;
	}

	// Release the vertex buffer.
	if (vertexBuffer_)
	{
		vertexBuffer_->Release();
		vertexBuffer_ = 0;
	}

	return;
}


bool BitMap::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ((positionX == previousPosX_) && (positionY == previousPosY_))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	previousPosX_ = positionX;
	previousPosY_ = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((screenWidth_ / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
  right = float( left + (float)bitmapWidth_ * scale_);

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(screenHeight_ / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
  bottom = float(top - (float)bitmapHeight_ * scale_);

	// Create the vertex array.
	vertices = new VertexType[vertexCount_];
	if (!vertices)
	{
		return false;
	}

	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * vertexCount_));

	// Unlock the vertex buffer.
	deviceContext->Unmap(vertexBuffer_, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitMap::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool BitMap::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	texture_ = new Texture();
	if (!texture_)
	{
		return false;
	}

	// Initialize the texture object.
	result = texture_->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool BitMap::LoadCameraStream(ID3D11Device* device, ARiftControl* arift_control)
{
	bool result;

	// Create the texture object.
	texture_ = new Texture();
	if (!texture_)
	{
		return false;
	}

	// Initialize the texture object.
  result = texture_->InitCameraStream(device, arift_control->camInput_);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitMap::ReleaseTexture()
{
	// Release the texture object.
	if (texture_)
	{
		texture_->Shutdown();
		delete texture_;
		texture_ = 0;
	}

	return;
}