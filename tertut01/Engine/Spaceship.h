#pragma once
#include "PhysicsObject.h"

/// Represents a spaceship in the game world.
/// The `Spaceship` class inherits from `PhysicsObject` and provides functionality
/// for controlling the spaceship's movement, rotation, and physics-based interactions.
class Spaceship : public PhysicsObject
{
public:
    /// Constructor to initialize the spaceship at a given position.
    /// @param pos The initial position of the spaceship in world space.
    Spaceship(const DirectX::SimpleMath::Vector3& pos);

    /// Applies thrust to move the spaceship forward in its current facing direction.
    /// @param force The amount of thrust to apply (overridden by the class's `thrustForce`).
    void ApplyThrust(float force);

    /// Applies torque to rotate the spaceship around its Y-axis.
    /// @param torque The amount of torque to apply (overridden by the class's `rotationSpeed`).
    void ApplyRotation(float torque);

    /// Forces the spaceship to rotate in place by a specified number of degrees.
    /// @param degrees The amount of rotation to apply in degrees.
    void ForceRotateInPlace(float degrees);

    /// Applies a braking force to reduce the spaceship's velocity.
    /// @param amount The amount of braking force to apply.
    void Brake(float amount);

    /// Retrieves the current position of the spaceship in world space.
    /// @return A `Vector3` representing the spaceship's position.
    DirectX::SimpleMath::Vector3 GetPosition() const;

    /// Retrieves the current rotation of the spaceship around the Y-axis.
    /// @return The rotation angle in degrees.
    float GetRotation() const;

    float thrustForce = 30.0f;     ///< Default thrust force for the spaceship.
    float rotationSpeed = 25.0f;  ///< Default rotation speed for the spaceship (degrees per second).
};