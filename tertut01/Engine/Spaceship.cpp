#include "pch.h"
#include "Spaceship.h"

using namespace DirectX::SimpleMath;

/// Constructor to initialize the spaceship at a given position.
/// Sets up the collision shape, mass, and physics properties of the spaceship.
/// @param pos The initial position of the spaceship in world space.
Spaceship::Spaceship(const DirectX::SimpleMath::Vector3& pos)
{
    // Define the collision shape as a box with specified dimensions.
    m_collisionShape = new btBoxShape(btVector3(35.0f, 35.0f, 45.0f)); // Size of spaceship

    // Set the initial transform (position and orientation) of the spaceship.
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

    // Define the mass and inertia of the spaceship.
    btScalar mass = 1.0f; // Mass of the spaceship
    btVector3 inertia(0, 0, 0);
    m_collisionShape->calculateLocalInertia(mass, inertia);
    inertia *= 0.5f; // Reduce inertia for more responsive movement.

    // Create the motion state and rigid body for the spaceship.
    m_motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_motionState, m_collisionShape, inertia);
    m_rigidBody = new btRigidBody(rbInfo);

    // Set damping to reduce linear and angular motion over time.
    m_rigidBody->setDamping(0.2f, 0.05f);

    // Prevent the spaceship from deactivating in the physics simulation.
    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

/// Applies thrust to move the spaceship forward in its current facing direction.
/// @param force The amount of thrust to apply (overridden by the class's `thrustForce`).
void Spaceship::ApplyThrust(float force)
{
    force = thrustForce; // Use the thrust force defined in the class.
    btVector3 thrustDirection = m_rigidBody->getWorldTransform().getBasis() * btVector3(0, 0, 1); // Forward direction.
    m_rigidBody->applyCentralForce(thrustDirection * force); // Apply the thrust force.
}

/// Applies torque to rotate the spaceship around its Y-axis.
/// @param torque The amount of torque to apply (overridden by the class's `rotationSpeed`).
void Spaceship::ApplyRotation(float torque)
{
    torque = rotationSpeed; // Use the rotation speed defined in the class.
    btVector3 torqueVec(0, torque, 0); // Y-axis rotation.
    m_rigidBody->applyTorque(torqueVec); // Apply the torque.

    // Adjust velocity to align with the spaceship's facing direction.
    btVector3 forwardDir = m_rigidBody->getWorldTransform().getBasis() * btVector3(0, 0, 1);
    btVector3 velocity = m_rigidBody->getLinearVelocity();
    btVector3 desiredVelocity = forwardDir * velocity.length();

    // Smoothly adjust velocity toward the desired direction.
    btVector3 steeringForce = (desiredVelocity - velocity) * 50.0f;
    m_rigidBody->applyCentralForce(steeringForce);

    // Clamp angular velocity to prevent excessive rotation.
    btVector3 angVel = m_rigidBody->getAngularVelocity();
    float maxTurnRate = 2.0f; // Maximum turn rate in radians per second.
    if (angVel.length() > maxTurnRate)
    {
        angVel = angVel.normalized() * maxTurnRate;
        m_rigidBody->setAngularVelocity(angVel);
    }
}

/// Forces the spaceship to rotate in place by a specified number of degrees.
/// @param degrees The amount of rotation to apply in degrees.
void Spaceship::ForceRotateInPlace(float degrees)
{
    btTransform trans = m_rigidBody->getWorldTransform();
    btQuaternion rot = trans.getRotation();

    // Create a quaternion representing the rotation.
    btQuaternion deltaRot(btVector3(0, 1, 0), DirectX::XMConvertToRadians(degrees));
    rot = deltaRot * rot; // Apply the rotation.
    rot.normalize(); // Normalize the quaternion.

    // Update the spaceship's transform with the new rotation.
    trans.setRotation(rot);
    m_rigidBody->setWorldTransform(trans);
    m_rigidBody->getMotionState()->setWorldTransform(trans);
}

/// Applies a braking force to reduce the spaceship's velocity.
/// @param amount The amount of braking force to apply.
void Spaceship::Brake(float amount)
{
    // Reduce linear velocity toward zero in the local forward direction.
    btVector3 currentVel = m_rigidBody->getLinearVelocity();
    btVector3 forward = m_rigidBody->getWorldTransform().getBasis() * btVector3(0, 0, 1);

    float speed = currentVel.dot(forward); // Project velocity onto the forward direction.

    if (speed > 0.01f)
    {
        // Apply a braking force opposite to the motion.
        btVector3 brakeForce = -forward.normalized() * amount;
        m_rigidBody->applyCentralForce(brakeForce);
    }
    else
    {
        // Stop drifting when near zero velocity.
        m_rigidBody->setLinearVelocity(btVector3(0, 0, 0));
    }

    // Reduce angular velocity to prevent spinning.
    btVector3 angVel = m_rigidBody->getAngularVelocity();
    angVel *= 0.9f; // Apply damping to angular velocity.
    m_rigidBody->setAngularVelocity(angVel);
}

/// Retrieves the current position of the spaceship in world space.
/// @return A `Vector3` representing the spaceship's position.
DirectX::SimpleMath::Vector3 Spaceship::GetPosition() const
{
    btTransform trans;
    m_rigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    return Vector3(pos.getX(), pos.getY(), pos.getZ());
}

/// Retrieves the current rotation of the spaceship around the Y-axis.
/// @return The rotation angle in degrees.
float Spaceship::GetRotation() const
{
    btTransform trans;
    m_rigidBody->getMotionState()->getWorldTransform(trans);
    btQuaternion rot = trans.getRotation();

    // Convert quaternion to yaw (Y-axis rotation).
    float yaw = btAtan2(2.0f * (rot.w() * rot.y() + rot.x() * rot.z()),
        1.0f - 2.0f * (rot.y() * rot.y() + rot.z() * rot.z()));

    return DirectX::XMConvertToDegrees(yaw); // Convert to degrees.
}