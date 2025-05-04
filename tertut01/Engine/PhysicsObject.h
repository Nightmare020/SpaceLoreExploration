#pragma once
#include <btBulletDynamicsCommon.h>
#include <SimpleMath.h>

/// Represents a physics object in the simulation.
/// This class encapsulates the Bullet Physics components required for a physics object,
/// including collision shape, rigid body, and motion state.
class PhysicsObject
{
public:
    /// Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~PhysicsObject();

    /// Adds the physics object to the specified dynamics world.
    /// @param world Pointer to the Bullet physics dynamics world.
    void AddToWorld(btDiscreteDynamicsWorld* world);

    /// Removes the physics object from the specified dynamics world.
    /// @param world Pointer to the Bullet physics dynamics world.
    void RemoveFromWorld(btDiscreteDynamicsWorld* world);

    /// Updates the transformation matrix of the object to synchronize physics and rendering.
    /// This method retrieves the current position and orientation from the physics engine
    /// and updates the world matrix used for rendering.
    virtual void UpdateTransform();

    /// Retrieves the rigid body associated with this physics object.
    /// @return Pointer to the Bullet rigid body.
    btRigidBody* GetRigidBody() const { return m_rigidBody; }

    /// Retrieves the world transformation matrix of the object.
    /// @return The transformation matrix in world space.
    DirectX::SimpleMath::Matrix GetWorldMatrix() const { return m_worldMatrix; }

protected:
    /// Pointer to the collision shape used for this object.
    /// Defines the physical shape of the object for collision detection.
    btCollisionShape* m_collisionShape = nullptr;

    /// Pointer to the rigid body representing the physical properties of the object.
    /// Includes mass, velocity, and other dynamics-related properties.
    btRigidBody* m_rigidBody = nullptr;

    /// Pointer to the motion state used to synchronize the physics engine and rendering.
    /// Handles the transformation updates between the physics world and the graphics world.
    btDefaultMotionState* m_motionState = nullptr;

    /// The transformation matrix representing the object's position and orientation in world space.
    /// Used for rendering the object in the correct location and orientation.
    DirectX::SimpleMath::Matrix m_worldMatrix = DirectX::SimpleMath::Matrix::Identity;
};