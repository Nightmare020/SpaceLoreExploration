#include "pch.h"
#include "Spaceship.h"

using namespace DirectX::SimpleMath;

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
	m_rigidBody->setDamping(0.2f, 0.3f); // Damping for linear and angular motion
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

void Spaceship::ForceRotateInPlace(float degrees)
{
	btTransform trans = m_rigidBody->getWorldTransform();
	btQuaternion rot = trans.getRotation();

	btQuaternion deltarRot(btVector3(0, 1, 0), DirectX::XMConvertToRadians(degrees));
	rot = deltarRot * rot;
	rot.normalize();

	trans.setRotation(rot);
	m_rigidBody->setWorldTransform(trans);
	m_rigidBody->getMotionState()->setWorldTransform(trans);
}

void Spaceship::Brake(float amount)
{
	// Reduce linear velocity toward zero (in local forward direction)
	btVector3 currentVel = m_rigidBody->getLinearVelocity();
	btVector3 forward = m_rigidBody->getWorldTransform().getBasis() * btVector3(0, 0, 1);

	float speed = currentVel.dot(forward);

	if (speed > 0.01f)
	{
		// Apply braking force opposite to motion
		btVector3 brakeForce = -forward.normalized() * amount;
		m_rigidBody->applyCentralForce(brakeForce);
	}
	else
	{
		// Stop drifting near zero
		m_rigidBody->setLinearVelocity(btVector3(0, 0, 0));
	}

	// Also reduce angular velocity
	btVector3 angVel = m_rigidBody->getAngularVelocity();
	angVel *= 0.9f; // Damping
	m_rigidBody->setAngularVelocity(angVel);
}

DirectX::SimpleMath::Vector3 Spaceship::GetPosition() const
{
	btTransform trans;
	m_rigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();
	return Vector3(pos.getX(), pos.getY(), pos.getZ());
}

float Spaceship::GetRotation() const
{
	btTransform trans;
	m_rigidBody->getMotionState()->getWorldTransform(trans);
	btQuaternion rot = trans.getRotation();

	// Convert quaternion to yaw (Y-axis rotation)
	float yaw = btAtan2(2.0f * (rot.w() * rot.y() + rot.x() * rot.z()),
		1.0f - 2.0f * (rot.y() * rot.y() + rot.z() * rot.z()));

	return DirectX::XMConvertToDegrees(yaw); // Convert to degrees
}
