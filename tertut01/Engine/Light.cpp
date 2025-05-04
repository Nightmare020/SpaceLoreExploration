// Light class implementation
// Holds data that represents a single light source.
#include "pch.h"
#include "light.h"

/// Constructor to initialize the light with default values.
Light::Light()
{
    m_ambientColour = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    m_diffuseColour = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    m_direction = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    m_specularColour = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    m_specularPower = 0.0f;
    m_position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    m_lookAt = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

/// Destructor.
Light::~Light()
{
}

/// Sets the ambient color of the light.
void Light::setAmbientColour(float red, float green, float blue, float alpha)
{
    m_ambientColour = DirectX::SimpleMath::Vector4(red, green, blue, alpha);
}

/// Sets the diffuse color of the light.
void Light::setDiffuseColour(float red, float green, float blue, float alpha)
{
    m_diffuseColour = DirectX::SimpleMath::Vector4(red, green, blue, alpha);
}

/// Sets the direction of the light (for directional lights).
void Light::setDirection(float x, float y, float z)
{
    m_direction = DirectX::SimpleMath::Vector3(x, y, z);
}

/// Sets the specular color of the light.
void Light::setSpecularColour(float red, float green, float blue, float alpha)
{
    m_specularColour = DirectX::SimpleMath::Vector4(red, green, blue, alpha);
}

/// Sets the specular power of the light.
void Light::setSpecularPower(float power)
{
    m_specularPower = power;
}

/// Sets the position of the light (for point lights).
void Light::setPosition(float x, float y, float z)
{
    m_position = XMVectorSet(x, y, z, 1.0f);
}

/// Retrieves the ambient color of the light.
DirectX::SimpleMath::Vector4 Light::getAmbientColour()
{
    return m_ambientColour;
}

/// Retrieves the diffuse color of the light.
DirectX::SimpleMath::Vector4 Light::getDiffuseColour()
{
    return m_diffuseColour;
}

/// Retrieves the direction of the light.
DirectX::SimpleMath::Vector3 Light::getDirection()
{
    return m_direction;
}

/// Retrieves the specular color of the light.
DirectX::SimpleMath::Vector4 Light::getSpecularColour()
{
    return m_specularColour;
}

/// Retrieves the specular power of the light.
float Light::getSpecularPower()
{
    return m_specularPower;
}

/// Retrieves the position of the light.
DirectX::SimpleMath::Vector3 Light::getPosition()
{
    return m_position;
}

/// Sets the look-at point of the light (deprecated, rarely used).
void Light::setLookAt(float x, float y, float z)
{
    m_lookAt = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
}