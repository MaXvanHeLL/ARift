#ifndef _MODEL_H_
#define _MODEL_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "Camera.h"
#include "Texture.h"
#include <chrono>
#include <vector>

using namespace DirectX;
class Model
{
public:
  struct State {
    float positionX_ = 0.0f;
    float positionY_ = 0.0f;
    float positionZ_ = 0.0f;

    float rotationX_ = 0.0f;
    float rotationY_ = 0.0f;
    float rotationZ_ = 0.0f;

    float scale_x_ = 1.0f;
    float scale_y_ = 1.0f;
    float scale_z_ = 1.0f;

    bool autoRotate_ = false;
  };

private:

  struct Face {
    int vertexIndices[3];
    int normalIndices[3];
    int textureIndices[3];
    bool hasNormals = false;
    bool hasTextures = false;
  };

  struct Vertex3D {
    float x;
    float y;
    float z;
  };

  struct Vertex2D {
    float x;
    float y;
  };

  Model::State oldState_;
  Model::State currentState_;

  ID3D11Buffer* vertexBuffer_;
  ID3D11Buffer* indexBuffer_;
  int vertexCount_;
  int indexCount_;
  Texture* texture_;

  struct VertexType
  {
    XMFLOAT3 position;
    XMFLOAT2 texture;
		XMFLOAT3 normal;
  };

  struct ModelType
  {
    float x, y, z;
    float tu, tv;
    float nx, ny, nz;
  };

  ModelType* modeltype_;
  std::vector<std::pair<Camera::Pose3D, std::chrono::duration<double> > > keyFrames_;
  std::chrono::time_point<std::chrono::system_clock> animationStartTime_;
  bool isAnimated_ = false;
  bool animationRunning_ = false;
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
  void SaveState();
  void SetState(Model::State newState);
  Model::State GetCurrentState() { return currentState_; };
  void RestoreState();
  XMMATRIX GetModelTransformation();
  void AddKeyFrame(
    Camera::Pose3D newKeyFrame,
    std::chrono::duration<double> timeSinceLastFrame = std::chrono::duration<double>(2)); // default 2 second
  void StartAnimation();
  void StopAnimation();
  void Animate();
private:

  bool InitializeBuffers(ID3D11Device*);

  bool LoadModel(char*);
  bool ReadObjFile(char* filename);
  bool ReadCustomTxt(char* filename);
  void ReleaseModel();

  void ShutdownBuffers();
  void RenderBuffers(ID3D11DeviceContext*);

  bool LoadTexture(ID3D11Device*, WCHAR*);
  void ReleaseTexture();
};
#endif