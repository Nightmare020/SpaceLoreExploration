#include "pch.h"
#include "Planet.h"

/// Constructor to initialize the planet with a given position and radius.
/// Sets up the collision shape, motion state, and rigid body for the planet.
/// @param pos The initial position of the planet in world space.
/// @param radius The radius of the planet.
Planet::Planet(const DirectX::SimpleMath::Vector3& pos, float radius)
{
    // Define the collision shape as a sphere with the specified radius.
    m_collisionShape = new btSphereShape(radius);

    // Set the initial transform (position and orientation) of the planet.
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

    // Create the motion state for the planet.
    m_motionState = new btDefaultMotionState(startTransform);

    // Define the rigid body construction info for a static body (mass = 0).
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, m_motionState, m_collisionShape);
    m_rigidBody = new btRigidBody(rbInfo);

    // Prevent the planet from deactivating in the physics simulation.
    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

/// Retrieves the radius of the planet.
/// @return The radius of the planet as a float.
float Planet::GetRadius() const
{
    // Cast the collision shape to a sphere shape and return its radius.
    return static_cast<btSphereShape*>(m_collisionShape)->getRadius();
}