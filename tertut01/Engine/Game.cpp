//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <random>

//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ImGui;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
	m_gameStarted = false;
	m_orbitCenter = Vector3(0.0f, 0.0f, 0.0f); // Center of orbit for the planet
}

Game::~Game()
{
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_dispatcher;
	delete m_collisionConfiguration;
	delete m_broadphase;

#ifdef DXTK_AUDIO
	if (m_audEngine)
	{
		m_audEngine->Suspend();
	}
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_input.Initialise(window);

	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	//setup imgui.  its up here cos we need the window handle too
	//pulled from imgui directx11 example
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window);		//tie to our window
	ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());	//tie to directx

	m_fullscreenRect.left = 0;
	m_fullscreenRect.top = 0;
	m_fullscreenRect.right = 800;
	m_fullscreenRect.bottom = 600;

	m_CameraViewRect.left = 500;
	m_CameraViewRect.top = 0;
	m_CameraViewRect.right = 800;
	m_CameraViewRect.bottom = 240;

	//setup light
	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(2.0f, 1.0f, 1.0f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera01.setPosition(Vector3(0.0f, 0.0f, 4.0f));
	m_Camera01.setRotation(Vector3(-90.0f, -180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

	// setup spaceship movement
	m_SpaceshipPosition = Vector3(0.0f, 0.0f, -70.0f);
	m_SpaceshipRotation = 60.0f;
	m_showFlames = false;

	//setup physics
	m_broadphase = new btDbvtBroadphase();
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0, 0, 0));

	// Create a spaceship object
	m_spaceship = std::make_unique<Spaceship>(m_SpaceshipPosition);
	m_spaceship->AddToWorld(m_dynamicsWorld);

	// Create sun object
	Vector3 sunPosition(0.0f, 0.0f, 0.0f);
	m_sun = std::make_unique<Planet>(sunPosition, 1.0f);
	m_sun->AddToWorld(m_dynamicsWorld);

	// Create planet object
	float semiMajorAxis = 103.0f; // X axis
	float semiMinorAxis = 99.0f; // Z axis

	std::random_device rd;
	std::mt19937 rng(rd()); // Seed with a real random value, if available

	// Random angle between 0 and 2*PI
	std::uniform_real_distribution<float> angleDist(0.0f, XM_2PI);
	float angle = angleDist(rng);

	// Calculate position in XZ plane around orbit center
	float x = m_orbitCenter.x + semiMajorAxis * cosf(angle);
	float z = m_orbitCenter.z + semiMinorAxis * sinf(angle);
	float y = m_orbitCenter.y; // Keep y constant

	Vector3 haloPosition(x, y, z);
	m_planet = std::make_unique<Planet>(haloPosition, 0.5f);

	// Pick a random texture
	std::uniform_int_distribution<int> dist(0, static_cast<int>(m_allPlanetTextures.size()) - 1);
	int textureIndex = dist(rng);
	m_planet->SetTexture(m_allPlanetTextures[textureIndex]);
	m_planet->AddToWorld(m_dynamicsWorld);

	// Create the procedural planetary system

#ifdef DXTK_AUDIO
	// Create DirectXTK for Audio objects
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif

	m_audEngine = std::make_unique<AudioEngine>(eflags);

	m_audioEvent = 0;
	m_audioTimerAcc = 10.f;
	m_retryDefault = false;

	m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

	m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
	m_effect1 = m_soundEffect->CreateInstance();
	m_effect2 = m_waveBank->CreateInstance(10);

	m_effect1->Play(true);
	m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game

	//Update all game objects
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	//Render all game content. 
	Render();

#ifdef DXTK_AUDIO
	// Only update audio engine once per frame
	if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
	{
		// Setup a retry in 1 second
		m_audioTimerAcc = 1.f;
		m_retryDefault = true;
	}
#endif


}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//this is hacky,  i dont like this here.  
	auto device = m_deviceResources->GetD3DDevice();

	// Get mouse state
	auto mouseDelta = m_input.getMouseDelta();

	// Check if start game (TAB) is pressed
	if (m_gameInputCommands.startGame)
	{
		m_gameStarted = !m_gameStarted; // Toggle game state
	}

	// -- Free Camera Mode --
	if (!m_gameStarted)
	{
		//note that currently.  Delta-time is not considered in the game object movement. 
		if (m_gameInputCommands.left)
		{
			Vector3 position = m_Camera01.getPosition();
			position += m_Camera01.getRight() * m_Camera01.getMoveSpeed();
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.right)
		{
			Vector3 position = m_Camera01.getPosition();
			position -= m_Camera01.getRight() * m_Camera01.getMoveSpeed();
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.forward)
		{
			Vector3 position = m_Camera01.getPosition(); //get the position
			position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed()); //add the forward vector
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.back)
		{
			Vector3 position = m_Camera01.getPosition(); //get the position
			position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed()); //add the forward vector
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.moveUp)
		{
			Vector3 position = m_Camera01.getPosition(); //get the position
			position.y += m_Camera01.getMoveSpeed(); //add the forward vector
			m_Camera01.setPosition(position);
		}
		if (m_gameInputCommands.moveDown)
		{
			Vector3 position = m_Camera01.getPosition(); //get the position
			position.y -= m_Camera01.getMoveSpeed(); //add the forward vector
			m_Camera01.setPosition(position);
		}

		// Handle camera rotation when righ mouse button is down
		if (m_gameInputCommands.rightMouseDown)
		{
			float rotationSpeed = 0.25f; // Rotation sensitivity
			Vector3 rotation = m_Camera01.getRotation();

			rotation.y -= mouseDelta.x * rotationSpeed * 0.01f;
			rotation.x -= mouseDelta.y * (rotationSpeed * 0.01f * 0.8f); // reduce pitch speed a bit

			// Clamp putch (c rotation) between -89 and 89 degrees to avoid flipping
			if (rotation.x > 89.0f) rotation.x = 89.0f;
			if (rotation.x < -89.0f) rotation.x = -89.0f;

			m_Camera01.setRotation(rotation);
		}
	}
	
	if (m_gameStarted)
	{
		// -- Gameplay Mode: Control the spaceship --

		btVector3 velocity = m_spaceship->GetRigidBody()->getLinearVelocity();
		float speed = velocity.length();

		// Detect movement to show flames
		m_showFlames = m_gameInputCommands.forward || m_gameInputCommands.left || m_gameInputCommands.right;

		if (m_gameInputCommands.forward)
		{
			// Move spaceship forward in its current facing direction
			m_spaceship->ApplyThrust(30.0f);
		}
		if (m_gameInputCommands.back)
		{
			// Move spaceship backward
			m_spaceship->Brake(30.0f);
		}
		if (m_gameInputCommands.left)
		{
			if (speed < 0.1f)
			{
				m_spaceship->ForceRotateInPlace(0.5f); // Rotate in place
			}
			else
			{
				m_spaceship->ApplyRotation(25.0f); // Rotate left
			}

		}
		if (m_gameInputCommands.right)
		{
			if (speed < 0.1f)
			{
				m_spaceship->ForceRotateInPlace(-0.5f); // Rotate in place
			}
			else
			{
				m_spaceship->ApplyRotation(-25.0f); // Rotate right
			}
		}

		// Step simulation
		m_dynamicsWorld->stepSimulation(timer.GetElapsedSeconds(), 10);

		// Update spaceship to sync graphics
		m_spaceship->UpdateTransform();

		// Update camera to follow spaceship
		Vector3 spaceshipPos = m_spaceship->GetPosition();
		Matrix rotationMatrix = Matrix::CreateRotationY(XMConvertToRadians(m_spaceship->GetRotation()));
		Vector3 cameraOffset = Vector3(0.0f, 50.0f, -20.0f); // Slightly behind and above
		Vector3 cameraPosition = spaceshipPos + Vector3::Transform(cameraOffset, rotationMatrix);

		m_Camera01.setPosition(cameraPosition);

		// Look at spaceship
		Vector3 direction = spaceshipPos - cameraPosition;
		direction.Normalize();

		float pitch = asinf(direction.y);
		float yaw = atan2f(direction.x, direction.z);

		m_Camera01.setRotation(Vector3(pitch, yaw, 0.0f));

		if (m_planet)
		{
			m_orbitAngle += m_orbitSpeed * static_cast<float>(timer.GetElapsedSeconds());
			m_planetSpinAngle += m_planetSpinSpeed * static_cast<float>(timer.GetElapsedSeconds());
			if (m_planetSpinAngle > XM_2PI)
			{
				m_planetSpinAngle -= XM_2PI;
			}

			// Calculate new position using polar coordinates
			float x = m_orbitCenter.x + m_ellipseA * cosf(m_orbitAngle);
			float z = m_orbitCenter.z + m_ellipseB * sinf(m_orbitAngle);
			float y = m_orbitCenter.y; // Keep y constant

			// Update the transform manually in Bullet
			btTransform orbitTransform;
			orbitTransform.setIdentity();
			orbitTransform.setOrigin(btVector3(x, y, z));

			m_planet->GetRigidBody()->getMotionState()->setWorldTransform(orbitTransform);
			m_planet->GetRigidBody()->setWorldTransform(orbitTransform);
		}
	}

	m_Camera01.Update();	//camera update.

	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;

	/*create our UI*/
	SetupGUI();

#ifdef DXTK_AUDIO
	m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
	if (m_audioTimerAcc < 0)
	{
		if (m_retryDefault)
		{
			m_retryDefault = false;
			if (m_audEngine->Reset())
			{
				// Restart looping audio
				m_effect1->Play(true);
			}
		}
		else
		{
			m_audioTimerAcc = 4.f;

			m_waveBank->Play(m_audioEvent++);

			if (m_audioEvent >= 11)
				m_audioEvent = 0;
		}
	}
#endif


	if (m_input.Quit())
	{
		ExitGame();
	}

	//store the previous mouse state for delta calculations
	m_input.UpdatePreviousMouseState();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_sprites->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());

	// Draw fullscreen sprite
	m_sprites->Draw(m_textureStars.Get(),
		XMFLOAT2(0, 0),
		nullptr,
		Colors::White,
		0.0f,
		XMFLOAT2(0, 0),
		XMFLOAT2(float(m_deviceResources->GetOutputSize().right) / 1920.0f,
			float(m_deviceResources->GetOutputSize().bottom) / 1080.0f));

	m_sprites->End();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();

	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
	//	context->RSSetState(m_states->Wireframe());



	//setup and draw cube
	m_BasicShaderPair.EnableShader(context);

	//flame color
	DirectX::XMFLOAT4 flameColor(1.0f, 0.2f, 0.2f, 1.0f);

	Matrix spaceshipMatrix = m_spaceship->GetWorldMatrix();

	if (m_gameStarted)
	{
		m_BasicShaderPair.SetShaderParameters(context, &spaceshipMatrix, &m_view, &m_projection, &m_Light, m_SpaceShipModel.GetTexture(), true);
	}
	else
	{
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_SpaceShipModel.GetTexture(), true);
	}

	//draw spaceship
	m_SpaceShipModel.Render(context);

	//draw flames if moving
	if (m_showFlames)
	{
		m_BasicShaderPair.SetShaderParameters(context, &spaceshipMatrix, &m_view, &m_projection, &m_Light, nullptr, false, flameColor);
		m_TurboFlameLeftModel.Render(context);
		m_TurboFlameRightModel.Render(context);
	}

	// Get transform from Bullet for the planet
	btTransform transform;
	m_sun->GetRigidBody()->getMotionState()->getWorldTransform(transform);
	btVector3 origin = transform.getOrigin();

	// Get planet radius from its shape
	float radius = static_cast<btSphereShape*>(m_sun->GetRigidBody()->getCollisionShape())->getRadius();

	// Build planet world matrix from physics position and scale
	Vector3 planetPos(origin.getX(), origin.getY(), origin.getZ());
	float visualScale = radius; // Scale for rendering
	Matrix planetSun = Matrix::CreateScale(radius) * Matrix::CreateTranslation(planetPos);

	//draw sun
	m_BasicShaderPair.SetShaderParameters(context, &planetSun, &m_view, &m_projection, &m_Light, m_textureSun.Get(), true);
	m_SunModel.Render(context);

	//draw planet around the sun
	if (m_planet)
	{
		btTransform orbitTransform;
		m_planet->GetRigidBody()->getMotionState()->getWorldTransform(orbitTransform);
		btVector3 orbitOrigin = orbitTransform.getOrigin();
		float orbitRadius = static_cast<btSphereShape*>(m_planet->GetRigidBody()->getCollisionShape())->getRadius();

		Vector3 orbitPos(orbitOrigin.getX(), orbitOrigin.getY(), orbitOrigin.getZ());
		Matrix spinRotation = Matrix::CreateRotationY(m_planetSpinAngle);
		Matrix orbitWorld = Matrix::CreateScale(orbitRadius) * spinRotation * Matrix::CreateTranslation(orbitPos);

		m_BasicShaderPair.SetShaderParameters(context, &orbitWorld, &m_view, &m_projection, &m_Light, m_planet->GetTexture().Get(), true);
		m_PlanetModel.Render(context);
	}

	//draw planet halo
	DirectX::XMFLOAT4 planetHaloColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, nullptr, false, planetHaloColor);
	m_PlanetHaloModel.Render(context);

	//render our GUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	// Show the new frame.
	m_deviceResources->Present();
}


// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::Black);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
	m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
	m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
	if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
	{
		// Setup a retry in 1 second
		m_audioTimerAcc = 1.f;
		m_retryDefault = true;
	}
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
	width = 800;
	height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();

	m_states = std::make_unique<CommonStates>(device);
	m_fxFactory = std::make_unique<EffectFactory>(device);
	m_sprites = std::make_unique<SpriteBatch>(context);
	m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our test model
	m_BasicModel.InitializeSphere(device);
	m_BasicModel2.InitializeModel(device, "drone.obj");
	m_BasicModel3.InitializeBox(device, 10.0f, 0.1f, 10.0f);	//box includes dimensions

	//setup spaceship model
	m_SpaceShipModel.InitializeModel(device, "SpaceShip.obj");

	// Initialize turbo flames
	m_TurboFlameLeftModel.InitializeModel(device, "FuelTurboFlameLeft.obj");
	m_TurboFlameRightModel.InitializeModel(device, "FuelTurboFlameRight.obj");

	// Initialize planet
	m_SunModel.InitializeModel(device, "Planet.obj");
	m_PlanetModel.InitializeModel(device, "Planet.obj");
	m_PlanetHaloModel.InitializeModel(device, "PlanetHalo.obj");

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");

	//load Textures
	CreateDDSTextureFromFile(device, L"Material.001_Base_color.dds", nullptr, m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Material.001_Roughness.dds", nullptr, m_texture2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Material.001_Emissive.dds", nullptr, m_texture3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Material.001_Metallic.dds", nullptr, m_texture4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Material.001_Mixed_AO.dds", nullptr, m_texture5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Stars_bg.dds", nullptr, m_textureStars.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Material.001_Normal_DirectX.dds", nullptr, m_texture6.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Solarsystemscope_texture_2k_sun.dds", nullptr, m_textureSun.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Arid/Arid_01-1024x512.dds", nullptr, m_textureArid1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Arid/Arid_02-1024x512.dds", nullptr, m_textureArid2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Arid/Arid_03-1024x512.dds", nullptr, m_textureArid3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Arid/Arid_04-1024x512.dds", nullptr, m_textureArid4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Arid/Arid_05-1024x512.dds", nullptr, m_textureArid5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Barren/Barren_01-1024x512.dds", nullptr, m_textureBaren1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Barren/Barren_02-1024x512.dds", nullptr, m_textureBaren2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Barren/Barren_03-1024x512.dds", nullptr, m_textureBaren3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Barren/Barren_04-1024x512.dds", nullptr, m_textureBaren4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Barren/Barren_05-1024x512.dds", nullptr, m_textureBaren5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Dusty/Dusty_01-1024x512.dds", nullptr, m_textureDusty1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Dusty/Dusty_02-1024x512.dds", nullptr, m_textureDusty2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Dusty/Dusty_03-1024x512.dds", nullptr, m_textureDusty3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Dusty/Dusty_04-1024x512.dds", nullptr, m_textureDusty4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Dusty/Dusty_05-1024x512.dds", nullptr, m_textureDusty5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_01-1024x512.dds", nullptr, m_textureGaseous1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_02-1024x512.dds", nullptr, m_textureGaseous2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_03-1024x512.dds", nullptr, m_textureGaseous3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_04-1024x512.dds", nullptr, m_textureGaseous4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_05-1024x512.dds", nullptr, m_textureGaseous5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_06-1024x512.dds", nullptr, m_textureGaseous6.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_07-1024x512.dds", nullptr, m_textureGaseous7.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_08-1024x512.dds", nullptr, m_textureGaseous8.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_09-1024x512.dds", nullptr, m_textureGaseous9.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_10-1024x512.dds", nullptr, m_textureGaseous10.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_11-1024x512.dds", nullptr, m_textureGaseous11.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_12-1024x512.dds", nullptr, m_textureGaseous12.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_13-1024x512.dds", nullptr, m_textureGaseous13.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_14-1024x512.dds", nullptr, m_textureGaseous14.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_15-1024x512.dds", nullptr, m_textureGaseous15.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_16-1024x512.dds", nullptr, m_textureGaseous16.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_17-1024x512.dds", nullptr, m_textureGaseous17.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_18-1024x512.dds", nullptr, m_textureGaseous18.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_19-1024x512.dds", nullptr, m_textureGaseous19.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Gaseous/Gaseous_20-1024x512.dds", nullptr, m_textureGaseous20.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Grassland/Grassland_01-1024x512.dds", nullptr, m_textureGrassland1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Grassland/Grassland_02-1024x512.dds", nullptr, m_textureGrassland2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Grassland/Grassland_03-1024x512.dds", nullptr, m_textureGrassland3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Grassland/Grassland_04-1024x512.dds", nullptr, m_textureGrassland4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Grassland/Grassland_05-1024x512.dds", nullptr, m_textureGrassland5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Jungle/Jungle_01-1024x512.dds", nullptr, m_textureJungle1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Jungle/Jungle_02-1024x512.dds", nullptr, m_textureJungle2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Jungle/Jungle_03-1024x512.dds", nullptr, m_textureJungle3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Jungle/Jungle_04-1024x512.dds", nullptr, m_textureJungle4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Jungle/Jungle_05-1024x512.dds", nullptr, m_textureJungle5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Marshy/Marshy_01-1024x512.dds", nullptr, m_textureMarshy1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Marshy/Marshy_02-1024x512.dds", nullptr, m_textureMarshy2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Marshy/Marshy_03-1024x512.dds", nullptr, m_textureMarshy3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Marshy/Marshy_04-1024x512.dds", nullptr, m_textureMarshy4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Marshy/Marshy_05-1024x512.dds", nullptr, m_textureMarshy5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Martian/Martian_01-1024x512.dds", nullptr, m_textureMartian1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Martian/Martian_02-1024x512.dds", nullptr, m_textureMartian2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Martian/Martian_03-1024x512.dds", nullptr, m_textureMartian3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Martian/Martian_04-1024x512.dds", nullptr, m_textureMartian4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Martian/Martian_05-1024x512.dds", nullptr, m_textureMartian5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Methane/Methane_01-1024x512.dds", nullptr, m_textureMethane1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Methane/Methane_02-1024x512.dds", nullptr, m_textureMethane2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Methane/Methane_03-1024x512.dds", nullptr, m_textureMethane3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Methane/Methane_04-1024x512.dds", nullptr, m_textureMethane4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Methane/Methane_05-1024x512.dds", nullptr, m_textureMethane5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Sandy/Sandy_01-1024x512.dds", nullptr, m_textureSandy1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Sandy/Sandy_02-1024x512.dds", nullptr, m_textureSandy2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Sandy/Sandy_03-1024x512.dds", nullptr, m_textureSandy3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Sandy/Sandy_04-1024x512.dds", nullptr, m_textureSandy4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Sandy/Sandy_05-1024x512.dds", nullptr, m_textureSandy5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Snowy/Snowy_01-1024x512.dds", nullptr, m_textureSnowy1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Snowy/Snowy_02-1024x512.dds", nullptr, m_textureSnowy2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Snowy/Snowy_03-1024x512.dds", nullptr, m_textureSnowy3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Snowy/Snowy_04-1024x512.dds", nullptr, m_textureSnowy4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Snowy/Snowy_04-1024x512.dds", nullptr, m_textureSnowy5.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Snowy/Snowy_05-1024x512.dds", nullptr, m_textureTundra1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Tundra/Tundra_01-1024x512.dds", nullptr, m_textureTundra2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Tundra/Tundra_02-1024x512.dds", nullptr, m_textureTundra3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Tundra/Tundra_03-1024x512.dds", nullptr, m_textureTundra4.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Planets_Textures/Planet Textures 1024x512/Tundra/Tundra_04-1024x512.dds", nullptr, m_textureTundra5.ReleaseAndGetAddressOf());

	m_allPlanetTextures = {
		m_textureArid1.Get(),
		m_textureArid2.Get(),
		m_textureArid3.Get(),
		m_textureArid4.Get(),
		m_textureArid5.Get(),
		m_textureBaren1.Get(),
		m_textureBaren2.Get(),
		m_textureBaren3.Get(),
		m_textureBaren4.Get(),
		m_textureBaren5.Get(),
		m_textureDusty1.Get(),
		m_textureDusty2.Get(),
		m_textureDusty3.Get(),
		m_textureDusty4.Get(),
		m_textureDusty5.Get(),
		m_textureGaseous1.Get(),
		m_textureGaseous2.Get(),
		m_textureGaseous3.Get(),
		m_textureGaseous4.Get(),
		m_textureGaseous5.Get(),
		m_textureGaseous6.Get(),
		m_textureGaseous7.Get(),
		m_textureGaseous8.Get(),
		m_textureGaseous9.Get(),
		m_textureGaseous10.Get(),
		m_textureGaseous11.Get(),
		m_textureGaseous12.Get(),
		m_textureGaseous13.Get(),
		m_textureGaseous14.Get(),
		m_textureGaseous15.Get(),
		m_textureGaseous16.Get(),
		m_textureGaseous17.Get(),
		m_textureGaseous18.Get(),
		m_textureGaseous19.Get(),
		m_textureGaseous20.Get(),
		m_textureGrassland1.Get(),
		m_textureGrassland2.Get(),
		m_textureGrassland3.Get(),
		m_textureGrassland4.Get(),
		m_textureGrassland5.Get(),
		m_textureJungle1.Get(),
		m_textureJungle2.Get(),
		m_textureJungle3.Get(),
		m_textureJungle4.Get(),
		m_textureJungle5.Get(),
		m_textureMarshy1.Get(),
		m_textureMarshy2.Get(),
		m_textureMarshy3.Get(),
		m_textureMarshy4.Get(),
		m_textureMarshy5.Get(),
		m_textureMethane1.Get(),
		m_textureMethane2.Get(),
		m_textureMethane3.Get(),
		m_textureMethane4.Get(),
		m_textureMethane5.Get(),
		m_textureSandy1.Get(),
		m_textureSandy2.Get(),
		m_textureSandy3.Get(),
		m_textureSandy4.Get(),
		m_textureSandy5.Get(),
		m_textureSnowy1.Get(),
		m_textureSnowy2.Get(),
		m_textureSnowy3.Get(),
		m_textureSnowy4.Get(),
		m_textureSnowy5.Get(),
		m_textureTundra1.Get(),
		m_textureTundra2.Get(),
		m_textureTundra3.Get(),
		m_textureTundra4.Get(),
		m_textureTundra5.Get()
	};

	//Initialise Render to texture
	m_FirstRenderPass = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	auto size = m_deviceResources->GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	m_projection = Matrix::CreatePerspectiveFieldOfView(
		fovAngleY,
		aspectRatio,
		0.1f,
		1000.0f
	);
}

void Game::SetupGUI()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Planetary System Parameters");

	ImGui::End();
}


void Game::OnDeviceLost()
{
	m_states.reset();
	m_fxFactory.reset();
	m_sprites.reset();
	m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
	m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
#pragma endregion