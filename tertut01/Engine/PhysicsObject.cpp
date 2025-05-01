#include "pch.h"
#include "PhysicsObject.h"

PhysicsObject::~PhysicsObject()
{
	delete m_rigidBody;
	delete m_motionState;
	delete m_collisionShape;
}

void PhysicsObject::AddToWorld(btDiscreteDynamicsWorld* world)
{
	if (world && m_rigidBody)
	{
		world->addRigidBody(m_rigidBody);
	}
	else
	{
		throw std::runtime_error("Failed to add PhysicsObject to world: Invalid world or rigid body.");
	}
}

void PhysicsObject::RemoveFromWorld(btDiscreteDynamicsWorld* world)
{
	if (world && m_rigidBody)
	{
		world->removeRigidBody(m_rigidBody);
	}
	else
	{
		throw std::runtime_error("Failed to remove PhysicsObject from world: Invalid world or rigid body.");
	}
}

void PhysicsObject::UpdateTransform()
{
	btTransform transform;
	m_rigidBody->getMotionState()->getWorldTransform(transform);
	btVector3 origin = transform.getOrigin();
	btQuaternion rotation = transform.getRotation();

	using namespace DirectX::SimpleMath;
	Vector3 pos(origin.getX(), origin.getY(), origin.getZ());
	Quaternion orientation(rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW());

	m_worldMatrix = Matrix::CreateFromQuaternion(orientation) * Matrix::CreateTranslation(pos);
}
