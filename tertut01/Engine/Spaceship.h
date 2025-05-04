#pragma once
#include "PhysicsObject.h"

class Spaceship : public PhysicsObject
{
public:
	Spaceship(const DirectX::SimpleMath::Vector3& pos);

	void ApplyThrust(float force);
	void ApplyRotation(float torque);
	void ForceRotateInPlace(float degrees);
	void Brake(float amount);

	DirectX::SimpleMath::Vector3 GetPosition() const;
	float GetRotation() const;

	float thrustForce = 30.0f;     // Default thrust
	float rotationSpeed = 25.0f;   // Degrees/sec or similar scale
};

