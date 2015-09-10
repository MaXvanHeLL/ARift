#ifndef _MODEL_H_
#define _MODEL_H_

// #include "../include/Model.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Texture.h"

using namespace DirectX;

class Model
{
	private:
		ID3D11Buffer* vertexbuffer_;
		ID3D11Buffer* indexbuffer_;
		int vertexcount_;
		int indexcount_;
		Texture* texture_;

		struct VertexType
		{
			XMFLOAT3 position;
			XMFLOAT2 texture;
		};

		struct ModelType
		{
			float x, y, z;
			float tu, tv;
			float nx, ny, nz;
		};

		ModelType* modeltype_;

	public:
		Model();
		Model(const Model&);
		~Model();

    bool Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename);
    bool Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, float x, float y, float z);
		void Shutdown();
		// called from GraphicsAPI::Render()
		void Render(ID3D11DeviceContext*);
    bool ReInitializeBuffers(ID3D11Device* device);
		// GetIndexCount returns the number of indexes in the model. 
		// The color shader will need this information to draw this model. 
		int GetIndexCount();

		ID3D11ShaderResourceView* GetTexture();

    void Scale(float scale);
    void Scale(float scale_x, float scale_y, float scale_z);
    void Move(float x, float y, float z);
    bool auto_rotate_ = false;
    XMMATRIX GetModelTransformation();
	private:

    float positionX_ = 0.0f;
    float positionY_ = 0.0f;
    float positionZ_ = 0.0f;
    float rotationX_ = 0.0f;
    float rotationY_ = 0.0f;
    float rotationZ_ = 0.0f;
    bool true_scale_ = true;
    float scale_x_ = 1.0f;
    float scale_y_ = 1.0f;
    float scale_z_ = 1.0f;

		bool InitializeBuffers(ID3D11Device*);

		bool LoadModel(char*);
		void ReleaseModel();

		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext*);

		bool LoadTexture(ID3D11Device*, WCHAR*);
		void ReleaseTexture();
};

#endif