#include "pch.h"
#include "Planet.h"

Planet::Planet(const DirectX::SimpleMath::Vector3& pos, float radius)
{
	m_collisionShape = new btSphereShape(radius); // Sphere shape for the planet
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	m_motionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, m_motionState, m_collisionShape);
	m_rigidBody = new btRigidBody(rbInfo);
	m_rigidBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	m_rigidBody->setActivationState(DISABLE_DEACTIVATION); // So it keeps simulating
}
