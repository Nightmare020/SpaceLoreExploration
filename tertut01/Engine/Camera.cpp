#include "pch.h"
#include "Camera.h"

/// Constructor to initialize the camera with default values.
/// Sets the initial position, orientation, and movement/rotation speeds.
Camera::Camera()
{
    // Initialize orientation (pitch, yaw, roll).
    m_orientation.x = -90.0f; // Pitch (rotation around X-axis).
    m_orientation.y = 0.0f;   // Yaw (rotation around Y-axis).
    m_orientation.z = 0.0f;   // Roll (rotation around Z-axis, rarely used in first-person cameras).

    // Initialize position in world space.
    m_position.x = 0.0f;
    m_position.y = 0.0f;
    m_position.z = 0.0f;

    // Initialize internal vectors (used for calculations).
    m_lookat = DirectX::SimpleMath::Vector3::Zero; // Look target point.
    m_forward = DirectX::SimpleMath::Vector3::Zero; // Forward direction.
    m_right = DirectX::SimpleMath::Vector3::Zero;   // Right direction.

    // Set movement and rotation speeds.
    m_movespeed = 0.30f;
    m_camRotRate = 3.0f;

    // Force an initial update to calculate dependent values.
    Update();
}

/// Destructor.
Camera::~Camera()
{
}

/// Updates the camera's internal state.
/// Recalculates the forward, right, and look-at vectors, and updates the view matrix.
void Camera::Update()
{
    float pitch = m_orientation.x; // Pitch (rotation around X-axis).
    float yaw = m_orientation.y;   // Yaw (rotation around Y-axis).

    // Calculate the forward vector using spherical coordinates.
    m_forward.x = cosf(pitch) * sinf(yaw);
    m_forward.y = sinf(pitch);
    m_forward.z = cosf(pitch) * cosf(yaw);
    m_forward.Normalize();

    // Calculate the right vector as the cross product of the forward vector and the world up vector.
    m_right = m_forward.Cross(DirectX::SimpleMath::Vector3::UnitY);
    m_right.Normalize();

    // Update the look-at point (position + forward direction).
    m_lookat = m_position + m_forward;

    // Generate the view matrix using the position, look-at point, and world up vector.
    m_cameraMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_lookat, DirectX::SimpleMath::Vector3::UnitY);
}

/// Retrieves the camera's view matrix.
/// @return The view matrix representing the camera's position and orientation.
DirectX::SimpleMath::Matrix Camera::getCameraMatrix()
{
    return m_cameraMatrix;
}

/// Sets the camera's position in world space.
/// @param newPosition The new position of the camera.
void Camera::setPosition(DirectX::SimpleMath::Vector3 newPosition)
{
    m_position = newPosition;
}

/// Retrieves the camera's current position in world space.
/// @return The position of the camera.
DirectX::SimpleMath::Vector3 Camera::getPosition()
{
    return m_position;
}

/// Retrieves the forward direction vector of the camera.
/// @return The forward direction vector.
DirectX::SimpleMath::Vector3 Camera::getForward()
{
    return m_forward;
}

/// Retrieves the right direction vector of the camera.
/// @return The right direction vector, perpendicular to the forward and up vectors.
DirectX::SimpleMath::Vector3 Camera::getRight()
{
    // World up vector.
    DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

    // Calculate the right vector as the cross product of the up vector and the forward vector.
    DirectX::SimpleMath::Vector3 right = up.Cross(m_forward);
    right.Normalize();
    return right;
}

/// Sets the camera's orientation (pitch, yaw, roll).
/// @param newRotation The new orientation of the camera.
void Camera::setRotation(DirectX::SimpleMath::Vector3 newRotation)
{
    m_orientation = newRotation;
}

/// Retrieves the camera's current orientation (pitch, yaw, roll).
/// @return The orientation of the camera.
DirectX::SimpleMath::Vector3 Camera::getRotation()
{
    return m_orientation;
}

/// Retrieves the camera's movement speed.
/// @return The movement speed of the camera.
float Camera::getMoveSpeed()
{
    return m_movespeed;
}

/// Retrieves the camera's rotation speed.
/// @return The rotation speed of the camera.
float Camera::getRotationSpeed()
{
    return m_camRotRate;
}