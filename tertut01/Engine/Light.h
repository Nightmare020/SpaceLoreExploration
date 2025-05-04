/**
 * @file Light.h
 * @brief Defines the Light class, which stores properties of a light source for rendering.
 *
 * The Light class encapsulates ambient, diffuse, and specular lighting properties, as well as
 * the position and direction of the light. It supports both point lights and directional lights.
 * This class is part of a DirectX-based rendering framework.
 *
 * Original framework by Paul, modified by Matt in 2019.
 */

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <directxmath.h>

using namespace DirectX;

/// Represents a light source in the rendering system.
/// The Light class provides methods to set and retrieve lighting properties such as color, direction, and position.
class Light
{
public:
    /// Allocates memory for the Light object with 16-byte alignment.
    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    /// Frees memory allocated for the Light object.
    void operator delete(void* p)
    {
        _mm_free(p);
    }

    /// Constructor to initialize the light with default values.
    Light();

    /// Destructor.
    ~Light();

    // Setters

    /// Sets the ambient color of the light.
    /// @param red Red component (0.0f to 1.0f).
    /// @param green Green component (0.0f to 1.0f).
    /// @param blue Blue component (0.0f to 1.0f).
    /// @param alpha Alpha component (0.0f to 1.0f).
    void setAmbientColour(float red, float green, float blue, float alpha);

    /// Sets the diffuse color of the light.
    /// @param red Red component (0.0f to 1.0f).
    /// @param green Green component (0.0f to 1.0f).
    /// @param blue Blue component (0.0f to 1.0f).
    /// @param alpha Alpha component (0.0f to 1.0f).
    void setDiffuseColour(float red, float green, float blue, float alpha);

    /// Sets the direction of the light (for directional lights).
    /// @param x X component of the direction vector.
    /// @param y Y component of the direction vector.
    /// @param z Z component of the direction vector.
    void setDirection(float x, float y, float z);

    /// Sets the specular color of the light.
    /// @param red Red component (0.0f to 1.0f).
    /// @param green Green component (0.0f to 1.0f).
    /// @param blue Blue component (0.0f to 1.0f).
    /// @param alpha Alpha component (0.0f to 1.0f).
    void setSpecularColour(float red, float green, float blue, float alpha);

    /// Sets the specular power of the light.
    /// @param power The specular power (higher values create sharper highlights).
    void setSpecularPower(float power);

    /// Sets the position of the light (for point lights).
    /// @param x X coordinate of the position.
    /// @param y Y coordinate of the position.
    /// @param z Z coordinate of the position.
    void setPosition(float x, float y, float z);

    /// Sets the look-at point of the light (deprecated, rarely used).
    /// @param x X coordinate of the look-at point.
    /// @param y Y coordinate of the look-at point.
    /// @param z Z coordinate of the look-at point.
    void setLookAt(float x, float y, float z);

    // Getters

    /// Retrieves the ambient color of the light.
    /// @return A Vector4 representing the ambient color (RGBA).
    DirectX::SimpleMath::Vector4 getAmbientColour();

    /// Retrieves the diffuse color of the light.
    /// @return A Vector4 representing the diffuse color (RGBA).
    DirectX::SimpleMath::Vector4 getDiffuseColour();

    /// Retrieves the direction of the light.
    /// @return A Vector3 representing the direction vector.
    DirectX::SimpleMath::Vector3 getDirection();

    /// Retrieves the specular color of the light.
    /// @return A Vector4 representing the specular color (RGBA).
    DirectX::SimpleMath::Vector4 getSpecularColour();

    /// Retrieves the specular power of the light.
    /// @return The specular power as a float.
    float getSpecularPower();

    /// Retrieves the position of the light.
    /// @return A Vector3 representing the position of the light.
    DirectX::SimpleMath::Vector3 getPosition();

    // Direct access for ImGui tweaking (optional for debugging or UI integration).

    /// Provides direct access to the ambient color for tweaking.
    DirectX::SimpleMath::Vector4& AmbientColor() { return m_ambientColour; }

    /// Provides direct access to the diffuse color for tweaking.
    DirectX::SimpleMath::Vector4& DiffuseColor() { return m_diffuseColour; }

    /// Provides direct access to the specular color for tweaking.
    DirectX::SimpleMath::Vector4& SpecularColor() { return m_specularColour; }

    /// Provides direct access to the position for tweaking.
    DirectX::SimpleMath::Vector3& Position() { return m_position; }

    /// Provides direct access to the direction for tweaking.
    DirectX::SimpleMath::Vector3& Direction() { return m_direction; }

    /// Provides direct access to the specular power for tweaking.
    float& SpecularPower() { return m_specularPower; }

protected:
    DirectX::SimpleMath::Vector4 m_ambientColour; ///< Ambient color of the light (RGBA).
    DirectX::SimpleMath::Vector4 m_diffuseColour; ///< Diffuse color of the light (RGBA).
    DirectX::SimpleMath::Vector3 m_direction; ///< Direction of the light (for directional lights).
    DirectX::SimpleMath::Vector4 m_specularColour; ///< Specular color of the light (RGBA).
    float m_specularPower; ///< Specular power of the light (sharpness of highlights).
    DirectX::SimpleMath::Vector3 m_position; ///< Position of the light (for point lights).
    DirectX::SimpleMath::Vector4 m_lookAt; ///< Look-at point of the light (deprecated).
};

#endif