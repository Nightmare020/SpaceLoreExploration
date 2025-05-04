#pragma once

#include <memory>
#include <vector>
#include <random>
#include <unordered_map>
#include <SimpleMath.h>
#include <btBulletDynamicsCommon.h>

#include "Planet.h"
#include "modelclass.h"
#include "Light.h"
#include "Shader.h"

class PlanetarySystem
{
public:
	PlanetarySystem(ID3D11Device* device, btDiscreteDynamicsWorld* dynamicsWorld, 
		const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures,
		const DirectX::SimpleMath::Vector3& orbitCenter);

	void Update(float deltaTime, const DirectX::SimpleMath::Vector3& cameraPos);
	void Render(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection, Light light,
		Shader& shader, ModelClass& planetModel, ModelClass& haloModel);

	float orbitSpeed = 1.0f;    // Global orbit multiplier
	float rotationSpeed = 1.0f; // Global spin multiplier

private:
	struct OrbitingPlanet
	{
		std::unique_ptr<Planet> planet;
		std::unique_ptr<ModelClass> model;
		float orbitRadius;
		float orbitAngle;
		float orbitSpeed;
		float spinAngle;
		float spinSpeed;
	};

	btDiscreteDynamicsWorld* m_DynamicsWorld;
	const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& m_Textures;
	DirectX::SimpleMath::Vector3 m_OrbitCenter;

	std::unordered_map<int64_t, OrbitingPlanet> m_Planets;
	std::mt19937 m_rng;ID3D11Device* m_Device;

	float m_GenerationRadius = 1500.0f;
	float m_Spacing = 50.0f;

	void TryGeneratePlanet(int index);
	int GetPlanetIndex(float distance);
	float GetRandomFloat(float min, float max);
	int GetRandomInt(int min, int max);
};

