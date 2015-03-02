#ifndef _MODEL_H_
#define _MODEL_H_

#include "../include/Model.h"
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Model
{
	private:
		ID3D11Buffer* vertexbuffer_;
		ID3D11Buffer* indexbuffer_;
		int vertexcount_;
		int indexcount_;

		struct VertexType
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};

	public:
		Model();
		Model(const Model&);
		~Model();

		bool Initialize(ID3D11Device*);
		void Shutdown();
		// called from GraphicsAPI::Render()
		void Render(ID3D11DeviceContext*);

		// GetIndexCount returns the number of indexes in the model. 
		// The color shader will need this information to draw this model. 
		int GetIndexCount();

	private:
		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext*);
};

#endif