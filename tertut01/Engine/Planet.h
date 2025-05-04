#pragma once
#include "PhysicsObject.h"
#include "PerlinNoise.hpp"

/// Represents a planet in the game world.
/// The `Planet` class inherits from `PhysicsObject` and provides functionality
/// for defining a planet's physical properties, such as its position, radius, and texture.
class Planet : public PhysicsObject
{
public:
    /// Constructor to initialize the planet with a given position and radius.
    /// @param pos The initial position of the planet in world space.
    /// @param radius The radius of the planet.
    Planet(const DirectX::SimpleMath::Vector3& pos, float radius);

    /// Retrieves the texture associated with the planet.
    /// @return A `ComPtr` to the shader resource view of the planet's texture.
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture() const { return m_texture; }

    /// Sets the texture for the planet.
    /// @param texture A `ComPtr` to the shader resource view of the texture to set.
    void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture) { m_texture = texture; }

    /// Retrieves the radius of the planet.
    /// @return The radius of the planet as a float.
    float GetRadius() const;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture; ///< The texture associated with the planet.
};