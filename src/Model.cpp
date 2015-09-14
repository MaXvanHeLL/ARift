#include "../include/Model.h"
#include <iostream>
#include <fstream>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;

Model::Model()
{
	vertexBuffer_ = 0;
	indexBuffer_ = 0;
	texture_ = 0;
	modeltype_ = 0;
}


Model::Model(const Model& other)
{}


Model::~Model()
{}


bool Model::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
  if (!result) 
    return false;

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
  if (!result) 
    return false;

	return true;
}

bool Model::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, float x, float y, float z)
{
  bool result;

  // Load in the model data,
  result = LoadModel(modelFilename);
  if (!result)
  {
    return false;
  }
  Move(x, y, z);
  // Initialize the vertex and index buffer that hold the geometry for the triangle.
  result = InitializeBuffers(device);
  if (!result)
    return false;

  // Load the texture for this model.
  result = LoadTexture(device, textureFilename);
  if (!result)
    return false;

  oldState_ = currentState_;
  return true;
}

void Model::Move(float x, float y, float z)
{
  currentState_.positionX_ += x;
  currentState_.positionY_ += y;
  currentState_.positionZ_ += z;
}

bool Model::ReInitializeBuffers(ID3D11Device* device)
{
  if (!modeltype_)
    return false;
  ShutdownBuffers();
  bool result = InitializeBuffers(device);
  return result;
}

void Model::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}


void Model::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int Model::GetIndexCount()
{
	return indexCount_;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return texture_->GetTexture();
}

XMMATRIX Model::GetModelTransformation()
{
  // scaling
  XMMATRIX scalingMatrix = XMMatrixScaling(currentState_.scale_x_, currentState_.scale_y_, currentState_.scale_z_);
  // rotation
  XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(currentState_.rotationX_, currentState_.rotationY_, currentState_.rotationZ_);
  // translation
  XMMATRIX translationMatrix = XMMatrixTranslation(currentState_.positionX_, currentState_.positionY_, currentState_.positionZ_);
  XMMATRIX modelTransform = XMMatrixMultiply(scalingMatrix, rotationMatrix);
  modelTransform = XMMatrixMultiply(modelTransform, translationMatrix);
  return modelTransform;
}

void Model::Scale(float scale)
{
  Scale(scale, scale, scale);
}

void Model::Scale(float scale_x, float scale_y, float scale_z)
{
  currentState_.scale_x_ = scale_x;
  currentState_.scale_y_ = scale_y;
  currentState_.scale_z_ = scale_z;
}

void Model::SaveState()
{
  oldState_ = currentState_;
}

void Model::RestoreState()
{
  currentState_ = oldState_;
}

void Model::SetState(Model::State newState)
{
  currentState_ = newState;
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

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

	// Load the vertex array and index array with data.
	for (int i = 0; i < vertexCount_; i++)
	{
		vertices[i].position = XMFLOAT3(modeltype_[i].x, modeltype_[i].y, modeltype_[i].z);
		vertices[i].texture = XMFLOAT2(modeltype_[i].tu, modeltype_[i].tv);
		// vertices[i].normal = XMFLOAT3(modeltype_[i].nx, modeltype_[i].ny, modeltype_[i].nz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount_;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer_);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
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


bool Model::LoadTexture(ID3D11Device* device, WCHAR* filename)
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


bool Model::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> vertexCount_;

	// Set the number of indices to be the same as the vertex count.
	indexCount_ = vertexCount_;

	// Create the model using the vertex count that was read in.
	modeltype_ = new ModelType[vertexCount_];
	if (!modeltype_)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i<vertexCount_; i++)
	{
		fin >> modeltype_[i].x >> modeltype_[i].y >> modeltype_[i].z;
		fin >> modeltype_[i].tu >> modeltype_[i].tv;
	  fin >> modeltype_[i].nx >> modeltype_[i].ny >> modeltype_[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}



void Model::ShutdownBuffers()
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


void Model::ReleaseTexture()
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


void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
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


void Model::ReleaseModel()
{
	if (modeltype_)
	{
		delete[] modeltype_;
		modeltype_ = 0;
	}

	return;
}