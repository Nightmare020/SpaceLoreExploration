#include "pch.h"
#include "PlanetarySystem.h"

PlanetarySystem::PlanetarySystem(btDiscreteDynamicsWorld* dynamicsWorld, std::vector<ID3D11ShaderResourceView*>& textures, const DirectX::SimpleMath::Vector3& orbitCenter)
	: m_DynamicsWorld(dynamicsWorld), m_Textures(textures), m_OrbitCenter(orbitCenter)
{
	std::random_device rd;
	m_rng = std::mt19937(rd()); // Seed with a real random value, if available
}

void PlanetarySystem::Initialize(size_t planetCount)
{
	float baseRadius = 120.0f;
	float spacing = 50.0f;

	for (size_t i = 0; i < planetCount; ++i)
	{
		float orbitRadius = baseRadius + spacing * i;
		float angle = GetRandomFloat(0.0f, XM_2PI);
		float orbitSpeed = GetRandomFloat(0.05f, 0.2f);
		float spinSpeed = GetRandomFloat(0.5f, 2.0f);
		float planetSize = GetRandomFloat(0.3f, 0.8f);

		float x = m_OrbitCenter.x + orbitRadius * cosf(angle);
		float z = m_OrbitCenter.z + orbitRadius * sinf(angle);
		float y = m_OrbitCenter.y; // Keep y constant

		DirectX::SimpleMath::Vector3 position(x, y, z);
		std::unique_ptr<Planet> planet = std::make_unique<Planet>(position, planetSize);

		// Assign random texture
		int textureIndex = GetRandomInt(0, static_cast<int>(m_Textures.size()) - 1);
		planet->SetTexture(m_Textures[textureIndex]);
		planet->AddToWorld(m_DynamicsWorld);

		OrbitingPlanet orbitingPlanet;
		orbitingPlanet.planet = std::move(planet);
		orbitingPlanet.orbitRadius = orbitRadius;
		orbitingPlanet.orbitAngle = angle;
		orbitingPlanet.orbitSpeed = orbitSpeed;
		orbitingPlanet.spinAngle = 0.0f;
		orbitingPlanet.spinSpeed = spinSpeed;

		m_Planets.push_back(std::move(orbitingPlanet));
	}
}

void PlanetarySystem::Update(float deltaTime)
{
	for (auto& orbitingPlanet : m_Planets)
	{
		orbitingPlanet.orbitAngle += orbitingPlanet.orbitSpeed * deltaTime;
		orbitingPlanet.spinAngle += orbitingPlanet.spinSpeed * deltaTime;
		if (orbitingPlanet.spinAngle > XM_2PI)
		{
			orbitingPlanet.spinAngle -= XM_2PI;
		}

		float x = m_OrbitCenter.x + orbitingPlanet.orbitRadius * cosf(orbitingPlanet.orbitAngle);
		float z = m_OrbitCenter.z + orbitingPlanet.orbitRadius * sinf(orbitingPlanet.orbitAngle);
		float y = m_OrbitCenter.y; // Keep y constant

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(x, y, z));

		orbitingPlanet.planet->GetRigidBody()->getMotionState()->setWorldTransform(transform);
		orbitingPlanet.planet->GetRigidBody()->setWorldTransform(transform);
	}
}

void PlanetarySystem::Render(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection, Light light, Shader& shader, ModelClass& planetModel)
{
	for (auto& orbitingPlanet : m_Planets)
	{
		btTransform transform;
		orbitingPlanet.planet->GetRigidBody()->getMotionState()->getWorldTransform(transform);
		btVector3 origin = transform.getOrigin();

		float radius = static_cast<btSphereShape*>(orbitingPlanet.planet->GetRigidBody()->getCollisionShape())->getRadius();
		DirectX::SimpleMath::Vector3 planetPos(origin.getX(), origin.getY(), origin.getZ());
		
		DirectX::SimpleMath::Matrix spinMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(orbitingPlanet.spinAngle);
		DirectX::SimpleMath::Matrix planetWorld = DirectX::SimpleMath::Matrix::CreateScale(radius) * spinMatrix * DirectX::SimpleMath::Matrix::CreateTranslation(planetPos);

		shader.SetShaderParameters(context, &planetWorld, &view, &projection, &light, orbitingPlanet.planet->GetTexture().Get(), true);
		planetModel.Render(context);
	}
}

float PlanetarySystem::GetRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(m_rng);
}

int PlanetarySystem::GetRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(m_rng);
}
