#pragma once

#include <memory>
#include <vector>
#include <random>
#include <SimpleMath.h>
#include <btBulletDynamicsCommon.h>

#include "Planet.h"
#include "modelclass.h"
#include "Light.h"
#include "Shader.h"

class PlanetarySystem
{
public:
	PlanetarySystem(btDiscreteDynamicsWorld* dynamicsWorld, 
		const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures,
		const DirectX::SimpleMath::Vector3& orbitCenter);

	void Initialize(size_t planetCount);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection, Light light,
		Shader& shader, ModelClass& planetModel, ModelClass& haloModel);

private:
	struct OrbitingPlanet
	{
		std::unique_ptr<Planet> planet;
		float orbitRadius;
		float orbitAngle;
		float orbitSpeed;
		float spinAngle;
		float spinSpeed;
	};

	btDiscreteDynamicsWorld* m_DynamicsWorld;
	const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& m_Textures;
	DirectX::SimpleMath::Vector3 m_OrbitCenter;

	std::vector<OrbitingPlanet> m_Planets;
	std::mt19937 m_rng;

	float GetRandomFloat(float min, float max);
	int GetRandomInt(int min, int max);
};

