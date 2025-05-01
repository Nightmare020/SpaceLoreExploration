#include "pch.h"
#include "Spaceship.h"

Spaceship::Spaceship(const DirectX::SimpleMath::Vector3& pos)
{
	m_collisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 2.0f)); // Size of spaceship
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	btScalar mass = 1.0f; // Mass of the spaceship
	btVector3 inertia(0, 0, 0);
	m_collisionShape->calculateLocalInertia(mass, inertia);
	m_motionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_motionState, m_collisionShape, inertia);
	m_rigidBody = new btRigidBody(rbInfo);
	m_rigidBody->setActivationState(DISABLE_DEACTIVATION); // So it keeps simulating
}

void Spaceship::ApplyThrust(float force)
{
	btVector3 thrustDirection = m_rigidBody->getWorldTransform().getBasis() *
		btVector3(0, 0, 1); // Forward direction
	m_rigidBody->applyCentralForce(thrustDirection * force);
}

void Spaceship::ApplyRotation(float torque)
{
	btVector3 torqueVec(0, torque, 0); // Y-axis rotation
	m_rigidBody->applyTorque(torqueVec);
}

DirectX::SimpleMath::Matrix Spaceship::GetWorldMatrix() const
{
	return DirectX::SimpleMath::Matrix();
}
