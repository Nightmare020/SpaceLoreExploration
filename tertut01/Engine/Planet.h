#pragma once
#include "PhysicsObject.h"

class Planet : public PhysicsObject
{
public:
	Planet(const DirectX::SimpleMath::Vector3& pos, float radius);
};

