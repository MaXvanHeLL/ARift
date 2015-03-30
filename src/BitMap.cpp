#include "../include/BitMap.h"
#include <iostream>


BitMap::BitMap()
{
	vertexbuffer_ = 0;
	indexbuffer_ = 0;
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
	screenwidth_ = screenWidth;
	screenheight_ = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	bitmapwidth_ = bitmapWidth;
	bitmapheight_ = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	previousposX_ = -1;
	previousposY_ = -1;

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

bool BitMap::InitializeCameras(ID3D11Device* device, int screenWidth, int screenHeight, ARiftControl* arift_control, int bitmapWidth, int bitmapHeight)
{
	bool result;

	// Store the screen size.
	screenwidth_ = screenWidth;
	screenheight_ = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	bitmapwidth_ = bitmapWidth;
	bitmapheight_ = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	previousposX_ = -1;
	previousposY_ = -1;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadCameraStream(device, arift_control);
	if (!result)
	{
		return false;
	}

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


bool BitMap::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, ARiftControl* arift_control)
{
	bool result = 0;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}
	
	result = texture_->Update(deviceContext, arift_control);
	if (!result)
	{
		std::cout << "Error: Could not Update Bitmap Object!" << std::endl;
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}


int BitMap::GetIndexCount()
{
	return indexcount_;
}


int BitMap::GetPositionX()
{
	return previousposX_;
}


int BitMap::GetPositionY()
{
	return previousposY_;
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
	vertexcount_ = 6;

	// Set the number of indices in the index array.
	indexcount_ = vertexcount_;

	// Create the vertex array.
	vertices = new VertexType[vertexcount_];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexcount_];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * vertexcount_));

	// Load the index array with data.
	for (i = 0; i < indexcount_; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexcount_;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexbuffer_);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexcount_;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexbuffer_);
	if (FAILED(result))
	{
		return false;
	}

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
	if (indexbuffer_)
	{
		indexbuffer_->Release();
		indexbuffer_ = 0;
	}

	// Release the vertex buffer.
	if (vertexbuffer_)
	{
		vertexbuffer_->Release();
		vertexbuffer_ = 0;
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
	if ((positionX == previousposX_) && (positionY == previousposY_))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	previousposX_ = positionX;
	previousposY_ = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((screenwidth_ / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)bitmapwidth_;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(screenheight_ / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)bitmapheight_;

	// Create the vertex array.
	vertices = new VertexType[vertexcount_];
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
	result = deviceContext->Map(vertexbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * vertexcount_));

	// Unlock the vertex buffer.
	deviceContext->Unmap(vertexbuffer_, 0);

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
	deviceContext->IASetVertexBuffers(0, 1, &vertexbuffer_, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexbuffer_, DXGI_FORMAT_R32_UINT, 0);

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
	result = texture_->InitCameraStream(device, arift_control);
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