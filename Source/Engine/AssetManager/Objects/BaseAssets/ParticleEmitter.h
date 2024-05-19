#pragma once
#include <Engine/AssetManager/ComponentSystem/Components/ParticleSystem.h>
#include <Engine/AssetManager/Objects/BaseAssets/BaseAsset.h>
#include <Engine/GraphicsEngine/Rendering/ParticleRenderer/ParticleVertex.h>

class ParticleSystem;
struct EmmiterSettingsData
{
	enum class eEmitterGeometry
	{
		cone,
		plane,
		cube,
		sphere
	};

	float SpawnRate = 1;
	float SpawnAngle = 0; // NotImplemented
	float LifeTime = 1;
	float SimulationSpeed = 1;
	int MaxParticles = 1000;
	eEmitterGeometry geometry = eEmitterGeometry::sphere; // NotImplemented

	Vector4f StartPosition = { 0,0,0,1 };
	Vector3f StartVelocity = { 0,0,0 };
	Vector3f EndVelocity; // NotImplemented
	Vector3f Acceleration = { 0,-9.82f,0 };

	float StartSize = 1;
	float EndSize = 1;

	Vector4f StartColor = { 1,1,1,1 };
	Vector4f EndColor = { 1,1,1,1 };

	bool InheriteTransform = true; // NotImplemented
	std::filesystem::path ParticleTexture;
};

struct ParticleEmitterTemplate
{
	std::filesystem::path Path;
	EmmiterSettingsData EmmiterSettings;
};

class ParticleEmitter : public AssetBase
{
private:
	MYLIB_REFLECTABLE();
	void InitParticle(Particlevertex& vertex) const;
	EmmiterSettingsData settings;
	//ComPtr<ID3D11Buffer> vertexBuffer;
	UINT stride;
	UINT offset;
	UINT primitiveTopology;

	std::vector<Particlevertex> particles;
	std::shared_ptr<TextureHolder> texture;
	float spawnedThisFrame = 0;
	float secondCounter = 0;
public:
	~ParticleEmitter() override = default;
	explicit ParticleEmitter(const std::filesystem::path& aFilePath);
	explicit ParticleEmitter(const ParticleEmitterTemplate& aTemplate);
	void Init() override;
	void Update(float aDeltaTime);
	virtual void Draw();
	virtual void SetAsResource() const;


	FORCEINLINE const EmmiterSettingsData& GetSettings() const { return settings; }
};

REFL_AUTO(type(ParticleEmitter))