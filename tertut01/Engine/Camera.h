#pragma once

/// Represents a simple camera for a DirectX application.
/// The camera provides basic functionality for position, orientation, and movement,
/// and generates a view matrix for rendering.
class Camera
{
public:
    /// Constructor to initialize the camera with default values.
    Camera();

    /// Destructor.
    ~Camera();

    /// Updates the camera's internal state, recalculating its view matrix and direction vectors.
    void Update();

    /// Retrieves the camera's view matrix.
    /// @return The view matrix representing the camera's position and orientation.
    DirectX::SimpleMath::Matrix getCameraMatrix();

    /// Sets the camera's position in world space.
    /// @param newPosition The new position of the camera.
    void setPosition(DirectX::SimpleMath::Vector3 newPosition);

    /// Retrieves the camera's current position in world space.
    /// @return The position of the camera.
    DirectX::SimpleMath::Vector3 getPosition();

    /// Retrieves the forward direction vector of the camera.
    /// @return The forward direction vector.
    DirectX::SimpleMath::Vector3 getForward();

    /// Retrieves the right direction vector of the camera.
    /// @return The right direction vector, perpendicular to the forward and up vectors.
    DirectX::SimpleMath::Vector3 getRight();

    /// Sets the camera's orientation (pitch, yaw, roll).
    /// @param newRotation The new orientation of the camera.
    void setRotation(DirectX::SimpleMath::Vector3 newRotation);

    /// Retrieves the camera's current orientation (pitch, yaw, roll).
    /// @return The orientation of the camera.
    DirectX::SimpleMath::Vector3 getRotation();

    /// Retrieves the camera's movement speed.
    /// @return The movement speed of the camera.
    float getMoveSpeed();

    /// Retrieves the camera's rotation speed.
    /// @return The rotation speed of the camera.
    float getRotationSpeed();

private:
    DirectX::SimpleMath::Matrix m_cameraMatrix; ///< The view matrix representing the camera's position and orientation.
    DirectX::SimpleMath::Vector3 m_lookat; ///< The point the camera is looking at.
    DirectX::SimpleMath::Vector3 m_position; ///< The position of the camera in world space.
    DirectX::SimpleMath::Vector3 m_forward; ///< The forward direction vector of the camera.
    DirectX::SimpleMath::Vector3 m_right; ///< The right direction vector of the camera.
    DirectX::SimpleMath::Vector3 m_up; ///< The up direction vector of the camera (world up).
    DirectX::SimpleMath::Vector3 m_orientation; ///< The orientation of the camera (pitch, yaw, roll).

    float m_movespeed; ///< The movement speed of the camera.
    float m_camRotRate; ///< The rotation speed of the camera.
};