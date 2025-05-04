//
// Game.h
//
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "Spaceship.h"
#include "Planet.h"
#include "PlanetarySystem.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>


/// Represents the main game class, which manages the game loop, rendering, input, and game objects.
/// This class initializes Direct3D resources, handles input, updates game objects, and renders the scene.
class Game final : public DX::IDeviceNotify
{
public:

    /// Constructor to initialize the game object.
    Game() noexcept(false);

    /// Destructor to clean up resources.
    ~Game();

    /// Initializes the game with the specified window and dimensions.
    /// @param window The handle to the game window.
    /// @param width The width of the game window.
    /// @param height The height of the game window.
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    /// Called when the Direct3D device is lost.   
    virtual void OnDeviceLost() override;

    /// Called when the Direct3D device is restored.
    virtual void OnDeviceRestored() override;

    // Message handlers
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);
#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    /// Retrieves the default window size.
    /// @param width Reference to store the default width.
    /// @param height Reference to store the default height.
    void GetDefaultSize(int& width, int& height) const;

private:

    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    /// Updates the game state.
    /// @param timer The timer object for tracking elapsed time.
    void Update(DX::StepTimer const& timer);

    /// Renders the game scene.
    void Render();

    /// Clears the back buffers.
    void Clear();

    /// Creates resources that depend on the Direct3D device.
    void CreateDeviceDependentResources();

    /// Creates resources that depend on the window size.
    void CreateWindowSizeDependentResources();

    /// Sets up the ImGui-based graphical user interface.
    void SetupGUI();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    //input manager. 
    Input									m_input;
    InputCommands							m_gameInputCommands;
    bool 								    m_gameStarted;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates> m_states; ///< Common rendering states.
    std::unique_ptr<DirectX::BasicEffect> m_batchEffect; ///< Effect for rendering primitives.
    std::unique_ptr<DirectX::EffectFactory> m_fxFactory; ///< Factory for creating effects.
    std::unique_ptr<DirectX::SpriteBatch> m_sprites; ///< Manages 2D sprite rendering.
    std::unique_ptr<DirectX::SpriteFont> m_font; ///< Font for rendering text.


    // Scene objects.
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch; ///< Batch for rendering primitives.
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_batchInputLayout; ///< Input layout for the batch.
    std::unique_ptr<DirectX::GeometricPrimitive> m_testmodel; ///< Test model for rendering.


    // Lighting.
    Light m_Light; ///< Represents the main light source.

    // Camera.
    Camera m_Camera01; ///< Main camera for the scene.

    //textures 
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture6;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureStars;

    /** PLANETS TEXTURES **/
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSun;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureArid1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureArid2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureArid3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureArid4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureArid5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureBaren1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureBaren2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureBaren3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureBaren4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureBaren5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureDusty1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureDusty2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureDusty3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureDusty4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureDusty5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous6;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous7;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous8;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous9;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous10;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous11;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous12;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous13;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous14;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous15;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous16;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous17;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous18;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous19;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGaseous20;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGrassland1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGrassland2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGrassland3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGrassland4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGrassland5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureJungle1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureJungle2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureJungle3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureJungle4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureJungle5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMarshy1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMarshy2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMarshy3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMarshy4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMarshy5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMartian1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMartian2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMartian3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMartian4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMartian5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMethane1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMethane2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMethane3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMethane4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMethane5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSandy1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSandy2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSandy3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSandy4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSandy5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSnowy1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSnowy2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSnowy3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSnowy4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSnowy5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureTundra1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureTundra2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureTundra3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureTundra4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureTundra5;

    //Shaders
    Shader																	m_BasicShaderPair;
	Shader																	m_GlowShaderPair;

    //Scene. 
    ModelClass																m_BasicModel;
    ModelClass																m_BasicModel2;
    ModelClass																m_BasicModel3;
    ModelClass																m_SpaceShipModel;
    ModelClass																m_TurboFlameLeftModel;
    ModelClass																m_TurboFlameRightModel;
    ModelClass																m_SunModel;
    ModelClass																m_PlanetModel;
    ModelClass																m_PlanetHaloModel;

    //RenderTextures
    RenderTexture* m_FirstRenderPass;
    RECT																	m_fullscreenRect;
    RECT																	m_CameraViewRect;

#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif


#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;
    DirectX::SimpleMath::Matrix                                             m_spaceShipWorld;

    // SPACESHIP MOVEMENT
    DirectX::SimpleMath::Vector3                                            m_SpaceshipPosition;
    float																    m_SpaceshipRotation;

    bool 																    m_showFlames;
    DirectX::SimpleMath::Matrix											    m_flameLeftWorld;
    DirectX::SimpleMath::Matrix											    m_flameRightWorld;

    // PHYSICS
	btDiscreteDynamicsWorld*                                                m_dynamicsWorld = nullptr;
	btBroadphaseInterface*                                                  m_broadphase = nullptr;
	btDefaultCollisionConfiguration*                                        m_collisionConfiguration = nullptr;
	btCollisionDispatcher*                                                  m_dispatcher = nullptr;
	btSequentialImpulseConstraintSolver*                                    m_solver = nullptr;

	std::unique_ptr<Spaceship>                                              m_spaceship;
	std::unique_ptr<Planet>                                                 m_sun;

    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>           m_allPlanetTextures;
	std::unique_ptr<PlanetarySystem>                                        m_planetarySystem;

    DirectX::SimpleMath::Vector3 											m_orbitCenter;
    DirectX::XMFLOAT4 m_glowColor;
    float m_glowThreshold;
    float m_glowIntensity;
};