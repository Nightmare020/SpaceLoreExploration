#include "pch.h"
#include "PlanetarySystem.h"

PlanetarySystem::PlanetarySystem(btDiscreteDynamicsWorld* dynamicsWorld, 
	const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures, const DirectX::SimpleMath::Vector3& orbitCenter)
	: m_DynamicsWorld(dynamicsWorld), m_Textures(textures), m_OrbitCenter(orbitCenter)
{
	std::random_device rd;
	m_rng = std::mt19937(rd()); // Seed with a real random value, if available
}

void PlanetarySystem::Update(float deltaTime, const DirectX::SimpleMath::Vector3& cameraPos)
{
	float distanceFromCenter = (cameraPos - m_OrbitCenter).Length();
	int centerIndex = GetPlanetIndex(distanceFromCenter);
	int range = static_cast<int>(m_GenerationRadius / m_Spacing);

	for (int i = centerIndex - range; i <= centerIndex + range; ++i)
	{
		if (i < 0) continue;
		TryGeneratePlanet(i);
	}

	for (auto& [index, orbitingPlanet] : m_Planets)
	{
		orbitingPlanet.orbitAngle += orbitingPlanet.orbitSpeed * deltaTime;
		orbitingPlanet.spinAngle += orbitingPlanet.spinSpeed * deltaTime;
		if (orbitingPlanet.spinAngle > XM_2PI)
		{
			orbitingPlanet.spinAngle -= XM_2PI;
		}

		float x = m_OrbitCenter.x + orbitingPlanet.orbitRadius * cos(orbitingPlanet.orbitAngle);
		float z = m_OrbitCenter.z + orbitingPlanet.orbitRadius * sin(orbitingPlanet.orbitAngle);
		float y = m_OrbitCenter.y;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(x, y, z));

		orbitingPlanet.planet->GetRigidBody()->getMotionState()->setWorldTransform(transform);
		orbitingPlanet.planet->GetRigidBody()->setWorldTransform(transform);
	}
}

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

		DirectX::SimpleMath::Matrix spinMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(orbitingPlanet.spinAngle);
		DirectX::SimpleMath::Matrix planetWorld = DirectX::SimpleMath::Matrix::CreateScale(radius) 
			* spinMatrix * DirectX::SimpleMath::Matrix::CreateTranslation(planetPos);

		shader.SetShaderParameters(context, &planetWorld, &view, &projection, &light, 
			orbitingPlanet.planet->GetTexture().Get(), true);
		planetModel.Render(context);

		float orbitScale = orbitingPlanet.orbitRadius / 170.0f;

		DirectX::SimpleMath::Matrix haloWorld = DirectX::SimpleMath::Matrix::CreateScale(orbitScale, 1.0f, orbitScale) *
			DirectX::SimpleMath::Matrix::CreateTranslation(m_OrbitCenter + DirectX::SimpleMath::Vector3(0, 0.1f, 0));
		DirectX::XMFLOAT4 haloColor(1.0f, 1.0f, 1.0f, 0.15f);
		shader.SetShaderParameters(context, &haloWorld, &view, &projection, &light, nullptr, false, haloColor);
		haloModel.Render(context);
	}
}

void PlanetarySystem::TryGeneratePlanet(int index)
{
	if (m_Planets.find(index) != m_Planets.end())
		return;

	float orbitRadius = 120.0f + index * m_Spacing;
	float angle = GetRandomFloat(0.0f, XM_2PI);
	float orbitSpeed = GetRandomFloat(0.01f, 0.04f);
	float spinSpeed = GetRandomFloat(0.5f, 2.0f);
	float planetSize = GetRandomFloat(0.3f, 0.8f);

	float x = m_OrbitCenter.x + orbitRadius * cosf(angle);
	float z = m_OrbitCenter.z + orbitRadius * sinf(angle);
	float y = m_OrbitCenter.y;

	DirectX::SimpleMath::Vector3 position(x, y, z);
	std::unique_ptr<Planet> planet = std::make_unique<Planet>(position, planetSize);

	int textureIndex = GetRandomInt(0, static_cast<int>(m_Textures.size()) - 1);
	planet->SetTexture(m_Textures[textureIndex].Get());
	planet->AddToWorld(m_DynamicsWorld);

	OrbitingPlanet orbitingPlanet;
	orbitingPlanet.planet = std::move(planet);
	orbitingPlanet.orbitRadius = orbitRadius;
	orbitingPlanet.orbitAngle = angle;
	orbitingPlanet.orbitSpeed = orbitSpeed;
	orbitingPlanet.spinAngle = 0.0f;
	orbitingPlanet.spinSpeed = spinSpeed;

	m_Planets[index] = std::move(orbitingPlanet);
}

int PlanetarySystem::GetPlanetIndex(float distance)
{
	return static_cast<int>((distance - 120.0f) / m_Spacing);
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
