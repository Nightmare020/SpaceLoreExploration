#pragma once
#include <btBulletDynamicsCommon.h>
#include <SimpleMath.h>

class PhysicsObject
{
public:
	virtual ~PhysicsObject();

	void AddToWorld(btDiscreteDynamicsWorld* world);
	void RemoveFromWorld(btDiscreteDynamicsWorld* world);
	virtual void UpdateTransform(); // sync physics to rendering

	btRigidBody* GetRigidBody() const { return m_rigidBody; }
	DirectX::SimpleMath::Matrix GetWorldMatrix() const { return m_worldMatrix; }

protected:
	btCollisionShape* m_collisionShape = nullptr;
	btRigidBody* m_rigidBody = nullptr;
	btDefaultMotionState* m_motionState = nullptr;
	DirectX::SimpleMath::Matrix m_worldMatrix = DirectX::SimpleMath::Matrix::Identity;
};

