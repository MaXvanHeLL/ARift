#include "../include/Model.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
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
  bool returnValue;

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (filename && fin.fail())
	{
    std::cout << "Error: could not open model file " << filename << std::endl;
		return false;
	}

  // Close the model file.
  fin.close();

  std::string filenameString = std::string(filename);
  std::string fileExtension = filenameString.substr((filenameString.length() - 4), filenameString.length());
  if (fileExtension == ".obj")
  {
    std::cout << "Reading model from *.obj file" << std::endl;
    returnValue = ReadObjFile(filename);
  }
  else // assume it is our custom format
  {
    returnValue = ReadCustomTxt(filename);
  }
	return true;
}
// Function to read in wavefront object files.
// Currently only very simple files can be read in by this function
// Restrictions:
// - Files may only contain one part
// - Textures are ignored, they need to be loaded manually
// - Materials are ignored
bool Model::ReadObjFile(char* filename)
{
  ifstream fin;
  
  // Open the model file.
  fin.open(filename);

  // If it could not open the file then exit.
  if (filename && fin.fail())
  {
    std::cout << "Error: could not open model file " << filename << std::endl;
    return false;
  }
  const int bufferSize = 256;
  char buffer[bufferSize];
  vector<Vertex3D> vertices;
  vector<Vertex3D> normals;
  vector<Vertex3D> textureCorrdinates;
  vector<Face> faces;
  // read in data
  int readLines = 0;
  while (fin.good())
  {
    fin.getline(buffer, bufferSize);
    readLines++;
    bool faceFound = false;
    vector<Vertex3D>* currentVector = NULL;
    // check for EOF
    if (fin.gcount() == 0)
      continue;
    char* block = strtok(buffer, " ");
    if (strncmp(block, "g", 1) == 0
      || strncmp(block, "usemtl", 6) == 0)
    {

      std::cout << "Could not convert file '" << filename << "'. File too complex." << std::endl;
      return false;
    }
    else if (strncmp(block, "f", 1) == 0)
    {
      // found a face definition
      faceFound = true;
    }
    else if (strncmp(block, "vn", 3) == 0)
    {
      // found a normal vector
      currentVector = &normals;
    }
    else if (strncmp(block, "vt", 3) == 0)
    {
      // found texture coordinates
      currentVector = &textureCorrdinates;
    }
    else if (strncmp(block, "v", 1) == 0)
    {
      // found a vertex
      currentVector = &vertices;
    }
    else
    {
      // found nothing of interest to us in this line
      continue;
    }
    float rawData[9];
    int i = 0;
    for (i = 0; i < 9 && block != NULL; i++)
    {
      block = strtok(NULL, " /");
      if (block != NULL)
        rawData[i] = (float)atof(block);
    }
    if (faceFound)
    {
      Face newFace;
      newFace.hasNormals = i > 4;
      newFace.hasTextures = i > 7;
      int k, j;
      for (k = j = 0; j < 3; j++)
      {
        newFace.vertexIndices[j] = (int)rawData[k++] - 1;
        if (newFace.hasTextures)
          newFace.textureIndices[j] = (int)rawData[k++] - 1;
        if (newFace.hasNormals)
          newFace.normalIndices[j] = (int)rawData[k++] - 1;
      }
      faces.push_back(newFace);
    }
    else
    {
      if (i > 4)
      {
        rawData[0] = rawData[0] / rawData[3];
        rawData[1] = rawData[1] / rawData[3];
        if (i > 3)
          rawData[2] = rawData[2] / rawData[3];
      }
      Vertex3D newData;
      newData.x = rawData[0];
      newData.y = rawData[1];
      if (i > 3)
        newData.z = rawData[2];

      currentVector->push_back(newData);
    }
  }
  fin.close();
  // convert to our Model format
  // each face should be a triangle so we need 3 modeltypes each
  indexCount_ = vertexCount_ = (faces.size() * 3);
  if (indexCount_ == 0)
  {
    std::cout << "Model file '" << filename 
              << "' was not in wavefront object format or did not contain any faces/triangles." << std::endl;
    return false;
  }
  modeltype_ = new ModelType[vertexCount_];
  int vertexCounter = 0;
  for (unsigned int faceCount = 0; faceCount < faces.size(); faceCount++)
  {
    Face currentFace = faces[faceCount];
    for (int i = 0; i < 3; i++)
    {
      modeltype_[vertexCounter].x = vertices[currentFace.vertexIndices[i]].x;
      modeltype_[vertexCounter].y = vertices[currentFace.vertexIndices[i]].y;
      modeltype_[vertexCounter].z = vertices[currentFace.vertexIndices[i]].z;
      if (currentFace.hasTextures)
      {
        modeltype_[vertexCounter].tu = textureCorrdinates[currentFace.textureIndices[i]].x;
        modeltype_[vertexCounter].tv = textureCorrdinates[currentFace.textureIndices[i]].y;
      }
      else
      {
        modeltype_[vertexCounter].tu = i == 1;
        modeltype_[vertexCounter].tv = i == 2;
      }
      if (currentFace.hasNormals)
      {
        modeltype_[vertexCounter].nx = normals[currentFace.normalIndices[i]].x;
        modeltype_[vertexCounter].ny = normals[currentFace.normalIndices[i]].y;
        modeltype_[vertexCounter].nz = normals[currentFace.normalIndices[i]].z;
        double norm = sqrt(modeltype_[vertexCounter].nx * modeltype_[vertexCounter].nx
                         + modeltype_[vertexCounter].ny * modeltype_[vertexCounter].ny
                         + modeltype_[vertexCounter].nz * modeltype_[vertexCounter].nz);
        modeltype_[vertexCounter].nx = (float)(modeltype_[vertexCounter].nx / norm);
        modeltype_[vertexCounter].ny = (float)(modeltype_[vertexCounter].ny / norm);
        modeltype_[vertexCounter].nz = (float)(modeltype_[vertexCounter].nz / norm);
      }
      vertexCounter++;
    }
  }
  return true;
}

bool Model::ReadCustomTxt(char* filename)
{
  ifstream fin;

  // Open the model file.
  fin.open(filename);

  // If it could not open the file then exit.
  if (filename && fin.fail())
  {
    std::cout << "Error: could not open model file " << filename << std::endl;
    return false;
  }

  char input;
  int i;
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
    fin.get(input);
    fin >> modeltype_[i].tu >> modeltype_[i].tv;
    fin.get(input);
    fin >> modeltype_[i].nx >> modeltype_[i].ny >> modeltype_[i].nz;
    fin.get(input);
  }
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