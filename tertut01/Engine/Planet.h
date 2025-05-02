#pragma once
#include "PhysicsObject.h"

class Planet : public PhysicsObject
{
public:
	Planet(const DirectX::SimpleMath::Vector3& pos, float radius);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture() const { return m_texture; }
	void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture) { m_texture = texture; }
	float GetRadius() const;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
};

