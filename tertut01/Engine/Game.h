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
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);
#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    // Properties
    void GetDefaultSize(int& width, int& height) const;

private:

    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    void Update(DX::StepTimer const& timer);
    void Render();
    void Clear();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
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
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

    // Scene Objects
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
    std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

    //lights
    Light																	m_Light;

    //Cameras
    Camera																	m_Camera01;

    //textures 
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture3;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture4;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture6;

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
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureStars;

    //Shaders
    Shader																	m_BasicShaderPair;

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
	std::unique_ptr<Planet>                                                 m_planet;

	float 																    m_orbitRadius = 10.0f;
    float                                                                   m_ellipseA = 105.0f; // semi-major axis (X)
	float                                                                   m_ellipseB = 100.0f; // semi-minor axis (Z)
	float 																    m_orbitSpeed = 0.1f; // Radians per second
	float 																    m_orbitAngle = 0.0f; // Current angle in orbit
	float                                                                   m_planetSpinAngle = 0.0f; // Current angle of planet spin
	float 																    m_planetSpinSpeed = 0.3f; // Radians per second
	DirectX::SimpleMath::Vector3 											m_orbitCenter;
};