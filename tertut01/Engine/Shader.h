#pragma once

#include "DeviceResources.h"
#include "Light.h"

/// Represents a shader object used in the rendering framework.
/// This class provides functionality to load, initialize, and manage shaders (vertex and pixel shaders),
/// as well as set shader parameters and enable shaders for rendering.
class Shader
{
public:
    /// Constructor to initialize the Shader object.
    Shader();

    /// Destructor to clean up resources.
    ~Shader();

    /// Initializes a standard vertex and pixel shader pair.
    /// @param device Pointer to the Direct3D device.
    /// @param vsFilename Path to the vertex shader file.
    /// @param psFilename Path to the pixel shader file.
    /// @return True if initialization is successful, false otherwise.
    bool InitStandard(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);

    /// Initializes a glow shader (vertex and pixel shader pair).
    /// @param device Pointer to the Direct3D device.
    /// @param vsFilename Path to the vertex shader file.
    /// @param psFilename Path to the pixel shader file.
    /// @return True if initialization is successful, false otherwise.
    bool InitGlowShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);

    /// Sets the parameters for the standard shader.
    /// @param context Pointer to the Direct3D device context.
    /// @param world Pointer to the world matrix.
    /// @param view Pointer to the view matrix.
    /// @param projection Pointer to the projection matrix.
    /// @param sceneLight1 Pointer to the light object.
    /// @param texture1 Pointer to the primary texture.
    /// @param useTexture Boolean indicating whether to use the texture.
    /// @param meshColor Color of the mesh (default is white).
    /// @param texture2 Optional secondary texture.
    /// @param texture3 Optional tertiary texture.
    /// @param texture4 Optional fourth texture.
    /// @param texture5 Optional fifth texture.
    /// @param texture6 Optional sixth texture.
    /// @return True if parameters are successfully set, false otherwise.
    bool SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, Light* sceneLight1,
        ID3D11ShaderResourceView* texture1,
        bool useTexture,
        DirectX::XMFLOAT4 meshColor = DirectX::XMFLOAT4(1, 1, 1, 1),
        ID3D11ShaderResourceView* texture2 = nullptr,
        ID3D11ShaderResourceView* texture3 = nullptr,
        ID3D11ShaderResourceView* texture4 = nullptr,
        ID3D11ShaderResourceView* texture5 = nullptr,
        ID3D11ShaderResourceView* texture6 = nullptr);

    /// Sets the parameters for the glow shader.
    /// @param context Pointer to the Direct3D device context.
    /// @param world Pointer to the world matrix.
    /// @param view Pointer to the view matrix.
    /// @param projection Pointer to the projection matrix.
    /// @param texture Pointer to the texture resource.
    /// @param glowColor Color of the glow effect.
    /// @param threshold Glow threshold value.
    /// @param intensity Glow intensity value.
    /// @return True if parameters are successfully set, false otherwise.
    bool SetGlowShaderParameters(ID3D11DeviceContext* context,
        DirectX::SimpleMath::Matrix* world,
        DirectX::SimpleMath::Matrix* view,
        DirectX::SimpleMath::Matrix* projection,
        ID3D11ShaderResourceView* texture,
        DirectX::XMFLOAT4 glowColor,
        float threshold,
        float intensity);

    /// Enables the shader for rendering.
    /// @param context Pointer to the Direct3D device context.
    void EnableShader(ID3D11DeviceContext* context);

private:
    /// Structure for the standard matrix buffer used in shaders.
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world; ///< World matrix.
        DirectX::XMMATRIX view; ///< View matrix.
        DirectX::XMMATRIX projection; ///< Projection matrix.
    };

    /// Structure for the light buffer used in shaders.
    struct LightBufferType
    {
        DirectX::SimpleMath::Vector4 ambient; ///< Ambient light color.
        DirectX::SimpleMath::Vector4 diffuse; ///< Diffuse light color.
        DirectX::SimpleMath::Vector3 position; ///< Light position.
        float padding; ///< Padding to align the structure to 16 bytes.

        DirectX::SimpleMath::Vector4 meshColor; ///< Color of the mesh.
        int useTexture; ///< Flag indicating whether to use a texture.
        DirectX::SimpleMath::Vector3 padding2; ///< Additional padding for alignment.
    };

    /// Structure for the glow buffer used in glow shaders.
    struct GlowBufferType
    {
        DirectX::XMFLOAT4 glowColor; ///< Color of the glow effect.
        float glowThreshold; ///< Threshold for the glow effect.
        float glowIntensity; ///< Intensity of the glow effect.
        DirectX::XMFLOAT2 padding; ///< Padding to align the structure to 16 bytes.
    };

    /// Structure for the camera buffer (not used in this implementation but can be extended).
    struct CameraBufferType
    {
        DirectX::SimpleMath::Vector3 cameraPosition; ///< Camera position in world space.
        float padding; ///< Padding to align the structure to 16 bytes.
    };

    // Shader resources
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader; ///< Vertex shader.
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader; ///< Pixel shader.
    ID3D11InputLayout* m_layout; ///< Input layout for the vertex shader.
    ID3D11Buffer* m_matrixBuffer; ///< Buffer for matrix data.
    ID3D11SamplerState* m_sampleState; ///< Sampler state for textures.
    ID3D11Buffer* m_lightBuffer; ///< Buffer for light data.
    ID3D11Buffer* m_glowBuffer; ///< Buffer for glow data.
};