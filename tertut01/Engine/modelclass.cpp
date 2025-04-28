////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "modelclass.h"

using namespace DirectX;

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

}
ModelClass::~ModelClass()
{
}


bool ModelClass::InitializeModel(ID3D11Device *device, char* filename)
{
	LoadModel(filename);

	m_vertexCount = preFabVertices.size();
	m_indexCount = preFabIndices.size();

	InitializeBuffers(device);

	// After initialising buffers, load textures if a material was found
	if (!m_diffuseTextureFilename.empty())
	{
		std::wstring wideTextureName(m_diffuseTextureFilename.begin(), m_diffuseTextureFilename.end());
		
		// Load the texture
		CreateDDSTextureFromFile(device, wideTextureName.c_str(), nullptr, m_diffuseTexture.ReleaseAndGetAddressOf());
	}

	if (!m_roughnessTextureFilename.empty())
	{
		std::wstring wideTextureName(m_roughnessTextureFilename.begin(), m_roughnessTextureFilename.end());

		// Load the texture
		CreateDDSTextureFromFile(device, wideTextureName.c_str(), nullptr, m_roughnessTexture.ReleaseAndGetAddressOf());
	}

	if (!m_metallicTextureFilename.empty())
	{
		std::wstring wideTextureName(m_metallicTextureFilename.begin(), m_metallicTextureFilename.end());

		// Load the texture
		CreateDDSTextureFromFile(device, wideTextureName.c_str(), nullptr, m_metallicTexture.ReleaseAndGetAddressOf());
	}

	if (!m_aoTextureFilename.empty())
	{
		std::wstring wideTextureName(m_aoTextureFilename.begin(), m_aoTextureFilename.end());

		// Load the texture
		CreateDDSTextureFromFile(device, wideTextureName.c_str(), nullptr, m_aoTexture.ReleaseAndGetAddressOf());
	}

	if (!m_normalTextureFilename.empty())
	{
		std::wstring wideTextureName(m_normalTextureFilename.begin(), m_normalTextureFilename.end());

		// Load the texture
		CreateDDSTextureFromFile(device, wideTextureName.c_str(), nullptr, m_normalTexture.ReleaseAndGetAddressOf());
	}

	if (!m_emissiveTextureFilename.empty())
	{
		std::wstring wideTextureName(m_emissiveTextureFilename.begin(), m_emissiveTextureFilename.end());

		// Load the texture
		CreateDDSTextureFromFile(device, wideTextureName.c_str(), nullptr, m_emissiveTexture.ReleaseAndGetAddressOf());
	}

	return true;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_diffuseTexture.Get();
}

bool ModelClass::InitializeTeapot(ID3D11Device* device)
{
	GeometricPrimitive::CreateTeapot(preFabVertices, preFabIndices, 1, 8, false);
	m_vertexCount	= preFabVertices.size();
	m_indexCount	= preFabIndices.size();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}
	return true;
}

bool ModelClass::InitializeSphere(ID3D11Device *device)
{
	GeometricPrimitive::CreateSphere(preFabVertices, preFabIndices, 1, 8, false);
	m_vertexCount = preFabVertices.size();
	m_indexCount = preFabIndices.size();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	return true;
}

bool ModelClass::InitializeBox(ID3D11Device * device, float xwidth, float yheight, float zdepth)
{
	GeometricPrimitive::CreateBox(preFabVertices, preFabIndices,
		DirectX::SimpleMath::Vector3(xwidth, yheight, zdepth),false);
	m_vertexCount = preFabVertices.size();
	m_indexCount = preFabIndices.size();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	return true;
}


void ModelClass::Shutdown()
{

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Bind the textures to pixel shader before drawing (if exists)
	if (m_diffuseTexture)
	{
		deviceContext->PSSetShaderResources(0, 1, m_diffuseTexture.GetAddressOf());
	}

	if (m_roughnessTexture)
	{
		deviceContext->PSSetShaderResources(1, 1, m_roughnessTexture.GetAddressOf());
	}

	if (m_metallicTexture)
	{
		deviceContext->PSSetShaderResources(2, 1, m_metallicTexture.GetAddressOf());
	}

	if (m_normalTexture)
	{
		deviceContext->PSSetShaderResources(3, 1, m_normalTexture.GetAddressOf());
	}

	if (m_emissiveTexture)
	{
		deviceContext->PSSetShaderResources(4, 1, m_emissiveTexture.GetAddressOf());
	}

	if (m_aoTexture)
	{
		deviceContext->PSSetShaderResources(5, 1, m_aoTexture.GetAddressOf());
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}
	
	// Load the vertex array and index array with data from the pre-fab
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position	= DirectX::SimpleMath::Vector3(preFabVertices[i].position.x, preFabVertices[i].position.y, preFabVertices[i].position.z);
		vertices[i].texture		= DirectX::SimpleMath::Vector2(preFabVertices[i].textureCoordinate.x, preFabVertices[i].textureCoordinate.y);
		vertices[i].normal		= DirectX::SimpleMath::Vector3(preFabVertices[i].normal.x, preFabVertices[i].normal.y, preFabVertices[i].normal.z);
	}
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = preFabIndices[i];
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ModelClass::LoadModel(char* filename)
{
	std::vector<XMFLOAT3> verts;
	std::vector<XMFLOAT3> norms;
	std::vector<XMFLOAT2> texCs;
	std::vector<unsigned int> faces;

	FILE* file;// = fopen(filename, "r");
	errno_t err;
	err = fopen_s(&file, filename, "r");
	if (err != 0)
		//if (file == NULL)
	{
		return false;
	}

	while (true)
	{
		char lineHeader[128];

		// Read first word of the line
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
		{
			break; // exit loop
		}
		else // Parse
		{
			if (strcmp(lineHeader, "v") == 0) // Vertex
			{
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) // Tex Coord
			{
				XMFLOAT2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				texCs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) // Normal
			{
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				norms.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) // Face
			{
				unsigned int face[9];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &face[0], &face[1], &face[2],
					&face[3], &face[4], &face[5],
					&face[6], &face[7], &face[8]);
				if (matches != 9)
				{
					// Parser error, or not triangle faces
					return false;
				}

				for (int i = 0; i < 9; i++)
				{
					faces.push_back(face[i]);
				}
			}
			// Handle mtllib entry to load the material file
			else if (strcmp(lineHeader, "mtllib") == 0)
			{
				char materialFile[128];
				fscanf_s(file, "%s\n", materialFile, sizeof(materialFile));
				LoadMaterial(materialFile);
			}
		}
	}

	int vIndex = 0, nIndex = 0, tIndex = 0;
	int numFaces = (int)faces.size() / 9;

	//// Create the model using the vertex count that was read in.
	m_vertexCount = numFaces * 3;
//	model = new ModelType[vertexCount];

	// "Unroll" the loaded obj information into a list of triangles.
	for (int f = 0; f < (int)faces.size(); f += 3)
	{
		int vertexIndex = faces[f + 0];
		int texCoordIndex = faces[f + 1];
		int normalIndex = faces[f + 2];

		VertexPositionNormalTexture tempVertex;

		// - Vertex
		tempVertex.position.x = verts[vertexIndex - 1].x;
		tempVertex.position.y = verts[vertexIndex - 1].y;
		tempVertex.position.z = verts[vertexIndex - 1].z;

		// - Texcoord
		if (!texCs.empty())
		{
			tempVertex.textureCoordinate.x = texCs[texCoordIndex - 1].x;
			tempVertex.textureCoordinate.y = 1.0f - texCs[texCoordIndex - 1].y;
		}
		else
		{
			tempVertex.textureCoordinate.x = 0.0f;
			tempVertex.textureCoordinate.y = 0.0f;
		}

		// - Normal
		if (!norms.empty())
		{
			tempVertex.normal.x = norms[normalIndex - 1].x;
			tempVertex.normal.y = norms[normalIndex - 1].y;
			tempVertex.normal.z = norms[normalIndex - 1].z;
		}
		else
		{
			tempVertex.normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}

		// Add the vertex
		preFabVertices.push_back(tempVertex);
		preFabIndices.push_back(vIndex);
		vIndex++;
	}

	m_indexCount = vIndex;

	verts.clear();
	norms.clear();
	texCs.clear();
	faces.clear();
	return true;
}


void ModelClass::ReleaseModel()
{
	return;
}

bool ModelClass::LoadMaterial(char* filename)
{
	// Load the material file
	FILE* file;
	errno_t err;
	err = fopen_s(&file, filename, "r");
	if (err != 0)
	{
		return false;
	}

	char line[256];
	while (fgets(line, sizeof(line), file))
	{
		std::string strLine(line);

		if (strLine.find("map_Kd") != std::string::npos) // Base color texture
		{
			m_diffuseTextureFilename = ParseTextureFilename(strLine, "map_Kd");
		}
		else if (strLine.find("map_Pr") != std::string::npos) // Roughness texture
		{
			m_roughnessTextureFilename = ParseTextureFilename(strLine, "map_Pr");
		}
		else if (strLine.find("map_Pm") != std::string::npos) // Metalness texture
		{
			m_metallicTextureFilename = ParseTextureFilename(strLine, "map_Pm");
		}
		else if (strLine.find("map_Ke") != std::string::npos) // Emissive texture
		{
			m_emissiveTextureFilename = ParseTextureFilename(strLine, "map_Ke");
		}
		else if (strLine.find("map_Bump") != std::string::npos) // Normal texture
		{
			m_normalTextureFilename = ParseTextureFilename(strLine, "map_Bump");
		}
	}
	fclose(file);
	return true;
}

std::string ModelClass::ParseTextureFilename(const std::string& line, const std::string& token)
{
	size_t pos = line.find(token);
	if (pos == std::string::npos)
	{
		return "";
	}

	pos += token.length();
	pos = line.find_first_not_of(" \t", pos);
	if (pos == std::string::npos)
	{
		return "";
	}

	std::string texturePath = line.substr(pos);
	texturePath.erase(std::remove(texturePath.begin(), texturePath.end(), '\n'), texturePath.end());
	texturePath.erase(std::remove(texturePath.begin(), texturePath.end(), '\r'), texturePath.end());
	return texturePath;
}