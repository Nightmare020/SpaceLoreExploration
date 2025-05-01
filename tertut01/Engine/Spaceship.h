#pragma once
#include "PhysicsObject.h"

class Spaceship : public PhysicsObject
{
public:
	Spaceship(const DirectX::SimpleMath::Vector3& pos);

	void ApplyThrust(float force);
	void ApplyRotation(float torque);
	DirectX::SimpleMath::Matrix GetWorldMatrix() const;
};

