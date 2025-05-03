////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "pch.h"
#include "PerlinNoise.hpp"
//#include <d3dx10math.h>
//#include <fstream>
//using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

class ModelClass
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};

public:
	ModelClass();
	~ModelClass();

	bool InitializeModel(ID3D11Device* device, char* filename);
	bool InitializeTeapot(ID3D11Device*);
	bool InitializeSphere(ID3D11Device*);
	bool InitializeBox(ID3D11Device*, float xwidth, float yheight, float zdepth);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool LoadPlanetModel(ID3D11Device* device, char* filename, siv::PerlinNoise& noise,
		float amplitude, float frequency);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadModel(char*);

	void ReleaseModel();

	// Load material (.mtl) file
	bool LoadMaterial(char*);
	std::string ParseTextureFilename(const std::string& line, const std::string& token);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;

	// Diffuse texture filename loaded from .mtl file
	std::string m_diffuseTextureFilename;
	std::string m_roughnessTextureFilename;
	std::string m_metallicTextureFilename;
	std::string m_aoTextureFilename;
	std::string m_normalTextureFilename;
	std::string m_emissiveTextureFilename;

	// Diffuse texture resource
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_diffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roughnessTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_metallicTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_aoTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_emissiveTexture;
};

#endif