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

		bool Initialize(ID3D11Device*, char*, WCHAR*);
		void Shutdown();
		// called from GraphicsAPI::Render()
		void Render(ID3D11DeviceContext*);

		// GetIndexCount returns the number of indexes in the model. 
		// The color shader will need this information to draw this model. 
		int GetIndexCount();

		ID3D11ShaderResourceView* GetTexture();

	private:
		bool InitializeBuffers(ID3D11Device*);

		bool LoadModel(char*);
		void ReleaseModel();

		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext*);

		bool LoadTexture(ID3D11Device*, WCHAR*);
		void ReleaseTexture();
};

#endif