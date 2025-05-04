#include "pch.h"
#include "PlanetarySystem.h"
#include "modelclass.h"

/// Constructor for the PlanetarySystem.
/// Initializes the random number generator and stores references to the device, dynamics world, textures, and orbit center.
PlanetarySystem::PlanetarySystem(ID3D11Device* device, btDiscreteDynamicsWorld* dynamicsWorld,
    const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures, const DirectX::SimpleMath::Vector3& orbitCenter)
    : m_Device(device), m_DynamicsWorld(dynamicsWorld), m_Textures(textures), m_OrbitCenter(orbitCenter)
{
    std::random_device rd;
    m_rng = std::mt19937(rd()); // Seed with a real random value, if available
}

/// Updates the state of the planetary system.
/// Generates new planets as needed and updates the position and rotation of existing planets.
void PlanetarySystem::Update(float deltaTime, const DirectX::SimpleMath::Vector3& cameraPos)
{
    float distanceFromCenter = (cameraPos - m_OrbitCenter).Length();
    int centerIndex = GetPlanetIndex(distanceFromCenter);
    int range = static_cast<int>(m_GenerationRadius / m_Spacing);

    // Generate planets within the visible range.
    for (int i = centerIndex - range; i <= centerIndex + range; ++i)
    {
        if (i < 0) continue;
        TryGeneratePlanet(i);
    }

    // Update the position and rotation of each planet.
    for (auto& [index, orbitingPlanet] : m_Planets)
    {
        orbitingPlanet.orbitAngle += orbitingPlanet.orbitSpeed * orbitSpeed * deltaTime;
        orbitingPlanet.spinAngle += orbitingPlanet.spinSpeed * rotationSpeed * deltaTime;
        if (orbitingPlanet.spinAngle > XM_2PI)
        {
            orbitingPlanet.spinAngle -= XM_2PI;
        }

        // Calculate the new position of the planet in its orbit.
        float x = m_OrbitCenter.x + orbitingPlanet.orbitRadius * cos(orbitingPlanet.orbitAngle);
        float z = m_OrbitCenter.z + orbitingPlanet.orbitRadius * sin(orbitingPlanet.orbitAngle);
        float y = m_OrbitCenter.y;

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(x, y, z));

        // Update the planet's physics transform.
        orbitingPlanet.planet->GetRigidBody()->getMotionState()->setWorldTransform(transform);
        orbitingPlanet.planet->GetRigidBody()->setWorldTransform(transform);
    }
}

/// Renders the planetary system.
/// Draws each planet and its associated halo.
void PlanetarySystem::Render(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix view,
    DirectX::SimpleMath::Matrix projection, Light light, Shader& shader, ModelClass& planetModel, ModelClass& haloModel)
{
    for (auto& [index, orbitingPlanet] : m_Planets)
    {
        btTransform transform;
        orbitingPlanet.planet->GetRigidBody()->getMotionState()->getWorldTransform(transform);
        btVector3 origin = transform.getOrigin();

        float radius = static_cast<btSphereShape*>(orbitingPlanet.planet->GetRigidBody()->getCollisionShape())->getRadius();
        DirectX::SimpleMath::Vector3 planetPos(origin.getX(), origin.getY(), origin.getZ());

        // Create the world matrix for the planet.
        DirectX::SimpleMath::Matrix spinMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(orbitingPlanet.spinAngle);
        DirectX::SimpleMath::Matrix planetWorld = DirectX::SimpleMath::Matrix::CreateScale(radius)
            * spinMatrix * DirectX::SimpleMath::Matrix::CreateTranslation(planetPos);

        // Render the planet.
        shader.SetShaderParameters(context, &planetWorld, &view, &projection, &light,
            orbitingPlanet.planet->GetTexture().Get(), true);
        orbitingPlanet.model->Render(context);

        // Render the halo around the planet.
        float orbitScale = orbitingPlanet.orbitRadius / 170.0f;
        DirectX::SimpleMath::Matrix haloWorld = DirectX::SimpleMath::Matrix::CreateScale(orbitScale, 1.0f / orbitScale, orbitScale) *
            DirectX::SimpleMath::Matrix::CreateTranslation(m_OrbitCenter + DirectX::SimpleMath::Vector3(0, 0.1f, 0));
        DirectX::XMFLOAT4 haloColor(1.0f, 1.0f, 1.0f, 0.15f);
        shader.SetShaderParameters(context, &haloWorld, &view, &projection, &light, nullptr, false, haloColor);
        haloModel.Render(context);
    }
}

/// Attempts to generate a planet at the specified orbit index.
/// Randomizes the planet's properties and adds it to the system.
void PlanetarySystem::TryGeneratePlanet(int index)
{
    if (m_Planets.find(index) != m_Planets.end())
        return;

    // Randomize properties.
    float orbitRadius = 120.0f + index * m_Spacing;
    float angle = GetRandomFloat(0.0f, XM_2PI);
    float orbitSpeed = GetRandomFloat(0.01f, 0.04f);
    float spinSpeed = GetRandomFloat(0.5f, 2.0f);
    float planetSize = GetRandomFloat(0.3f, 0.8f);

    // Calculate the initial position of the planet.
    float x = m_OrbitCenter.x + orbitRadius * cosf(angle);
    float z = m_OrbitCenter.z + orbitRadius * sinf(angle);
    float y = m_OrbitCenter.y;

    DirectX::SimpleMath::Vector3 position(x, y, z);

    // Create the planet physics object.
    std::unique_ptr<Planet> planet = std::make_unique<Planet>(position, planetSize);

    // Assign a random texture to the planet.
    int textureIndex = GetRandomInt(0, static_cast<int>(m_Textures.size()) - 1);
    planet->SetTexture(m_Textures[textureIndex].Get());

    // Add the planet to the Bullet physics world.
    planet->AddToWorld(m_DynamicsWorld);

    // Generate the planet's 3D model with procedural terrain.
    std::unique_ptr<ModelClass> planetModel = std::make_unique<ModelClass>();
    siv::PerlinNoise noise(GetRandomInt(0, 999999));
    float amplitude = 5.5f;
    float frequency = 3.0f;
    planetModel->LoadPlanetModel(m_Device, "Planet.obj", noise, amplitude, frequency);

    // Create and store the orbiting planet.
    OrbitingPlanet orbitingPlanet;
    orbitingPlanet.planet = std::move(planet);
    orbitingPlanet.model = std::move(planetModel);
    orbitingPlanet.orbitRadius = orbitRadius;
    orbitingPlanet.orbitAngle = angle;
    orbitingPlanet.orbitSpeed = orbitSpeed;
    orbitingPlanet.spinAngle = 0.0f;
    orbitingPlanet.spinSpeed = spinSpeed;

    m_Planets[index] = std::move(orbitingPlanet);
}

/// Calculates the orbit index of a planet based on its distance from the center.
int PlanetarySystem::GetPlanetIndex(float distance)
{
    return static_cast<int>((distance - 120.0f) / m_Spacing);
}

/// Generates a random float within the specified range.
float PlanetarySystem::GetRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_rng);
}

/// Generates a random integer within the specified range.
int PlanetarySystem::GetRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(m_rng);
}