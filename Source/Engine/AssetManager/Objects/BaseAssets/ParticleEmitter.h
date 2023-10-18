#pragma once
#include <Engine/GraphicsEngine/Rendering/ParticleRenderer/ParticleVertex.h>

struct EmmiterSettingsData
{
	enum emitterGeometry
	{
		cone,
		plane,
		cube,
		sphere
	};

	float SpawnRate = 1;
	float SpawnAngle = 0;
	float LifeTime = 1;
	float SimulationSpeed = 1;
	int MaxParticles = 1000;
	emitterGeometry geometry = sphere;

	Vector3f StartVelocity;
	Vector3f EndVelocity;

	Vector3f Acceleration;

	float StartSize;
	float EndSize;

	Vector4f StartColor;
	Vector4f EndColor;
};

struct ParticleEmitterTemplate
{
	std::filesystem::path Path;
	EmmiterSettingsData EmmiterSettings;
};

class ParticleEmitter : public AssetBase
{
private:
	EmmiterSettingsData data;
	ComPtr<ID3D11Buffer> vertexBuffer;
	UINT stride;
	UINT offset;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11GeometryShader> geometryShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	UINT primitiveTopology;
	ComPtr<ID3D11InputLayout> inputLayout;

	std::vector<Particlevertex> particles;

	void InitParticle(size_t aParticleIndex);
public:
	virtual ~ParticleEmitter();

	explicit ParticleEmitter(const std::filesystem::path& aFilePath);
	explicit ParticleEmitter(const ParticleEmitterTemplate& aTemplate);
	void Init() override; 

	virtual void Draw();
	virtual void SetAsResource() const;
	
	FORCEINLINE const EmmiterSettingsData& GetSettings() const { return data; }
};