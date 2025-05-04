#include "pch.h"
#include "PhysicsObject.h"

/// Destructor to clean up allocated resources.
/// Deletes the collision shape, motion state, and rigid body associated with the object.
PhysicsObject::~PhysicsObject()
{
    delete m_rigidBody;
    delete m_motionState;
    delete m_collisionShape;
}

/// Adds the physics object to the specified dynamics world.
/// Ensures that the world and rigid body are valid before adding.
/// Throws an exception if the operation fails.
/// @param world Pointer to the Bullet physics dynamics world.
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

/// Removes the physics object from the specified dynamics world.
/// Ensures that the world and rigid body are valid before removing.
/// Throws an exception if the operation fails.
/// @param world Pointer to the Bullet physics dynamics world.
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

/// Updates the transformation matrix of the object to synchronize physics and rendering.
/// Retrieves the current position and orientation from the physics engine and updates
/// the world matrix used for rendering.
void PhysicsObject::UpdateTransform()
{
    btTransform transform;
    m_rigidBody->getMotionState()->getWorldTransform(transform);

    // Extract position and rotation from the Bullet transform.
    btVector3 origin = transform.getOrigin();
    btQuaternion rotation = transform.getRotation();

    using namespace DirectX::SimpleMath;

    // Convert Bullet's position and rotation to DirectX::SimpleMath types.
    Vector3 pos(origin.getX(), origin.getY(), origin.getZ());
    Quaternion orientation(rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getW());

    // Update the world matrix with the new position and orientation.
    m_worldMatrix = Matrix::CreateFromQuaternion(orientation) * Matrix::CreateTranslation(pos);
}