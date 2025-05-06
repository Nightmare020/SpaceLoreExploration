#pragma once

#include <memory>
#include <vector>
#include <random>
#include <unordered_map>
#include <SimpleMath.h>
#include <btBulletDynamicsCommon.h>

#include "Planet.h"
#include "modelclass.h"
#include "Light.h"
#include "Shader.h"

/// Represents a system of orbiting planets.
/// This class manages the creation, updating, and rendering of planets in a dynamic planetary system.
class PlanetarySystem
{
public:
    /// Constructor for the PlanetarySystem.
    /// @param device Pointer to the Direct3D device used for rendering.
    /// @param dynamicsWorld Pointer to the Bullet physics dynamics world for managing physics.
    /// @param textures A collection of textures to be applied to planets.
    /// @param orbitCenter The center of the planetary system's orbit.
    PlanetarySystem(ID3D11Device* device, btDiscreteDynamicsWorld* dynamicsWorld,
        const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures,
        const DirectX::SimpleMath::Vector3& orbitCenter);

    /// Updates the state of the planetary system.
    /// @param deltaTime The time elapsed since the last update.
    /// @param cameraPos The position of the camera, used to determine which planets to generate.
    void Update(float deltaTime, const DirectX::SimpleMath::Vector3& cameraPos);

    /// Renders the planetary system.
    /// @param context The Direct3D device context used for rendering.
    /// @param view The view matrix for rendering.
    /// @param projection The projection matrix for rendering.
    /// @param light The light source used for rendering.
    /// @param shader The shader used for rendering planets and halos.
    /// @param planetModel The model used for rendering planets.
    /// @param haloModel The model used for rendering halos.
    void Render(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection, Light light,
        Shader& shader, ModelClass& planetModel, ModelClass& haloModel);

    /// Global multipliers for orbit and rotation speeds.
    float orbitSpeed = 1.0f;    ///< Multiplier for orbit speed of all planets.
    float rotationSpeed = 1.0f; ///< Multiplier for rotation speed of all planets.

    float m_noiseAmplitude = 5.5f;
    float m_noiseFrequency = 3.0f;

private:
    /// Represents a single orbiting planet in the system.
    struct OrbitingPlanet
    {
        std::unique_ptr<Planet> planet; ///< The planet object.
        std::unique_ptr<ModelClass> model; ///< The 3D model of the planet.
        float orbitRadius; ///< The radius of the planet's orbit.
        float orbitAngle; ///< The current angle of the planet in its orbit.
        float orbitSpeed; ///< The speed at which the planet orbits.
        float spinAngle; ///< The current spin angle of the planet.
        float spinSpeed; ///< The speed at which the planet spins.
    };

    btDiscreteDynamicsWorld* m_DynamicsWorld; ///< Pointer to the Bullet physics dynamics world.
    const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& m_Textures; ///< Collection of textures for planets.
    DirectX::SimpleMath::Vector3 m_OrbitCenter; ///< The center of the planetary system's orbit.

    std::unordered_map<int64_t, OrbitingPlanet> m_Planets; ///< Map of planets indexed by their orbit index.
    std::mt19937 m_rng; ///< Random number generator for procedural generation.
    ID3D11Device* m_Device; ///< Pointer to the Direct3D device.

    float m_GenerationRadius = 1500.0f; ///< Radius within which planets are generated.
    float m_Spacing = 50.0f; ///< Spacing between planets in their orbits.

    /// Attempts to generate a planet at the specified orbit index.
    /// @param index The index of the orbit where the planet should be generated.
    void TryGeneratePlanet(int index);

    /// Calculates the orbit index of a planet based on its distance from the center.
    /// @param distance The distance from the center of the planetary system.
    /// @return The calculated orbit index.
    int GetPlanetIndex(float distance);

    /// Generates a random float within the specified range.
    /// @param min The minimum value.
    /// @param max The maximum value.
    /// @return A random float between min and max.
    float GetRandomFloat(float min, float max);

    /// Generates a random integer within the specified range.
    /// @param min The minimum value.
    /// @param max The maximum value.
    /// @return A random integer between min and max.
    int GetRandomInt(int min, int max);
};