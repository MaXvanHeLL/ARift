#include "../include/Shader.h"
#include <iostream>

Shader::Shader()
{
	vertexshader_ = 0;
	pixelshader_ = 0;
	layout_ = 0;
	matrixbuffer_ = 0;
  undistortionBuffer_ = 0;
	samplestate_ = 0;
	lightBuffer_ = 0;
}

Shader::Shader(const Shader& other)
{}

Shader::~Shader()
{}

bool Shader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
  result = InitializeShader(device, hwnd, L"src/Light.vs", L"src/Light.ps", L"src/UndistortionShader.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void Shader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool Shader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4X4 worldMatrix,
  XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView* texture, UndistortionBuffer* undistBuffer,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
	if (!result)
	{
		return false;
	}
  if (undistBuffer != NULL)
  {
    result = SetUndistortionParameters(deviceContext, undistBuffer);
    if (!result)
      return false;
  }
	// Now render the prepared buffers with the shader.
  RenderShader(deviceContext, indexCount, (undistBuffer != NULL));

	return true;
}

bool Shader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4X4 worldMatrix,
	XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
  return Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, NULL, lightDirection, diffuseColor);
}

bool Shader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, WCHAR* undistShaderFilename)
{

	std::cout << "Vertex Shader file: " << vsFilename << std::endl;
  std::cout << "Pixel  Shader file: " << psFilename << std::endl;
  std::cout << "Pixel  Shader file: " << undistShaderFilename << std::endl;

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
  ID3D10Blob* undistortionShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
  D3D11_BUFFER_DESC undistortionBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;
  undistortionShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
  
  // Compile pixel shader code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

  // Compile undistortion shader code
  result = D3DCompileFromFile(undistShaderFilename, NULL, NULL, "UndistortionShader", "ps_5_0",
    D3D10_SHADER_ENABLE_STRICTNESS, 0, &undistortionShaderBuffer, &errorMessage);

  if (FAILED(result))
  {
    // If the shader failed to compile it should have written something to the error message.
    if (errorMessage)
    {
      OutputShaderErrorMessage(errorMessage, hwnd, undistShaderFilename);
    }
    // If there was  nothing in the error message then it simply could not find the file itself.
    else
    {
      MessageBox(hwnd, undistShaderFilename, L"Missing Shader File", MB_OK);
    }

    return false;
  }

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), NULL, &vertexshader_);
	if (FAILED(result))
		return false;

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), 
		pixelShaderBuffer->GetBufferSize(), NULL, &pixelshader_);
	if (FAILED(result))
		return false;

  // Create the pixel shader from the buffer.
  result = device->CreatePixelShader(undistortionShaderBuffer->GetBufferPointer(),
    undistortionShaderBuffer->GetBufferSize(), NULL, &undistortionShader_);
  if (FAILED(result))
    return false;

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// for Light-Shading, normal vector
	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &layout_);
	if (FAILED(result))
		return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

  undistortionShaderBuffer->Release();
  undistortionShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage =          D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth =      sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags =      D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixbuffer_);
	if (FAILED(result))
		return false;

  // Setup the description of the dynamic undistortion constant buffer that is in the undistortion shader.
  undistortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  undistortionBufferDesc.ByteWidth = sizeof(UndistortionBuffer);
  undistortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  undistortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  undistortionBufferDesc.MiscFlags = 0;
  undistortionBufferDesc.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the undistortion shader constant buffer from within this class.
  result = device->CreateBuffer(&undistortionBufferDesc, NULL, &undistortionBuffer_);
  if (FAILED(result))
    return false;

	// Create a texture sampler state description.
	samplerDesc.Filter =   D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &samplestate_);
	if (FAILED(result))
		return false;

	// Pay attention to the size of the constant buffers, if they are not multiples of 16 you need to pad extra space 
	// on to the end of them or the CreateBuffer function will fail.In this case the constant buffer is 28 bytes 
	// with 4 bytes padding to make it 32.

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer_);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Shader::ShutdownShader()
{
	// Release the light constant buffer.
	if (lightBuffer_)
	{
		lightBuffer_->Release();
		lightBuffer_ = 0;
	}

	// Release the sampler state.
	if (samplestate_)
	{
		samplestate_->Release();
		samplestate_ = 0;
	}

	// Release the matrix constant buffer.
	if (matrixbuffer_)
	{
		matrixbuffer_->Release();
		matrixbuffer_ = 0;
	}

	// Release the layout.
	if (layout_)
	{
		layout_->Release();
		layout_ = 0;
	}

	// Release the pixel shader.
	if (pixelshader_)
	{
		pixelshader_->Release();
		pixelshader_ = 0;
	}

	// Release the vertex shader.
	if (vertexshader_)
	{
		vertexshader_->Release();
		vertexshader_ = 0;
	}

  // Release the undistortion shader.
  if (undistortionShader_)
  {
    undistortionShader_->Release();
    undistortionShader_ = 0;
  }

	return;
}


void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool Shader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix,
	XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX worldMatrix_XmMat = XMLoadFloat4x4(&worldMatrix);
	worldMatrix_XmMat = XMMatrixTranspose(worldMatrix_XmMat);
	XMStoreFloat4x4(&worldMatrix, worldMatrix_XmMat);

	XMMATRIX viewMatrix_XmMat = XMLoadFloat4x4(&viewMatrix);
	viewMatrix_XmMat = XMMatrixTranspose(viewMatrix_XmMat);
	XMStoreFloat4x4(&viewMatrix, viewMatrix_XmMat);

	XMMATRIX projectionMatrix_XmMat = XMLoadFloat4x4(&projectionMatrix);
	projectionMatrix_XmMat = XMMatrixTranspose(projectionMatrix_XmMat);
	XMStoreFloat4x4(&projectionMatrix, projectionMatrix_XmMat);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(matrixbuffer_, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixbuffer_);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(lightBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(lightBuffer_, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer_);

	return true;
}

bool Shader::SetUndistortionParameters(ID3D11DeviceContext* deviceContext, UndistortionBuffer* undistBuffer)
{
  HRESULT result;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  UndistortionBuffer* dataPtr;
  unsigned int bufferNumber;
  // Lock the constant buffer so it can be written to.
  result = deviceContext->Map(undistortionBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result))
  {
    return false;
  }
  // Get a pointer to the data in the constant buffer.
  dataPtr = (UndistortionBuffer*)mappedResource.pData;
  memcpy(dataPtr, undistBuffer, sizeof(UndistortionBuffer));

  // Unlock the constant buffer.
  deviceContext->Unmap(undistortionBuffer_, 0);

  // Set the position of the constant buffer in the vertex shader.
  bufferNumber = 0;

  // Finanly set the constant buffer in the vertex shader with the updated values.
  deviceContext->PSSetConstantBuffers(bufferNumber, 1, &undistortionBuffer_);

  return true;
}

void Shader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, bool undistort)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout_);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexshader_, NULL, 0);
  if (undistort)
    deviceContext->PSSetShader(undistortionShader_, NULL, 0);
  else
    deviceContext->PSSetShader(pixelshader_, NULL, 0);
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &samplestate_);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
