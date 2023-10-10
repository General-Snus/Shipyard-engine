#pragma once
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp> 
#include <GraphicsEngine/Rendering/Buffers/FrameBuffer.h>
#include <GraphicsEngine/Rendering/Buffers/ObjectBuffer.h> 
#include <GraphicsEngine/Rendering/Buffers/LightBuffer.h> 
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h> 
#include <GraphicsEngine/Rendering/Buffers/LineBuffer.h> 
#include <GraphicsEngine/Rendering/Buffers/G_buffer.h> 
#include <ThirdParty\CU\Math.hpp>
#include <AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>

typedef CU::Matrix4x4<float>  Matrix;

class GraphicCommandBase
{
protected:
	FrameBuffer& GetFrameBuffer();
	ObjectBuffer& GetObjectBuffer();
	LineBuffer& GetLineBuffer();
	LightBuffer& GetLightBuffer();
	G_Buffer& GetGBuffer();
public:
	virtual ~GraphicCommandBase() = default;
	virtual void ExecuteAndDestroy() = 0;
	virtual void Destroy() = 0;
	GraphicCommandBase* Next;
};
class GraphicsCommandListIterator;
class GraphicsCommandList
{
	friend class GraphicsCommandListIterator;
public:
	GraphicsCommandList();
	~GraphicsCommandList();
	void Initialize(size_t aSize = megByte); //REFACTOR ADD byte

	template<typename CommandClass, typename ...Args>
	void AddCommand(Args ... arguments)
	{
		const size_t commandSize = sizeof(CommandClass);
		if (myCursor + commandSize > mySize)
		{
			throw std::out_of_range("CommandList is full");
		}
		GraphicCommandBase* ptr = reinterpret_cast<GraphicCommandBase*>(myData + myCursor);
		myCursor += commandSize;
		::new (ptr) CommandClass(arguments ...);
		*myLink = ptr;
		myLink = &ptr->Next;
	}

	void Execute();
	void Reset();

private:
	uint8_t* myData = nullptr;
	size_t myCursor = 0;
	size_t mySize = 0;

	GraphicCommandBase* myRoot = nullptr;
	GraphicCommandBase** myLink = nullptr;

	bool isExecuting = false;
	bool isFinished = false;
};
class GraphicsCommandListIterator
{
	GraphicCommandBase* myPtr = nullptr;
public:
	GraphicsCommandListIterator(const GraphicsCommandList& lst);
	GraphicCommandBase* Next();

	FORCEINLINE bool HasCommand() const 
	{
		return myPtr; 
	}
}; 





class GfxCmd_SetFrameBuffer : public GraphicCommandBase
{
private:
	Matrix myViewMatrix;
	Matrix myProjectionMatrix;
	Vector3f myPosition;
	float myDeltaTime;
	int RenderMode;
public:
	GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix, const Transform& ref, int aRenderMode);
	GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix, const Matrix& ref, int aRenderMode);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};

class GfxCmd_SetLightBuffer : public GraphicCommandBase
{
	std::vector<std::pair< DirectionalLight*, Texture*>> dirLight;
	std::vector<std::pair< PointLight, Texture*>> pointLight; //REFACTOR pointer
	std::vector<std::pair< SpotLight*, Texture*>> spotLight;
public:
	GfxCmd_SetLightBuffer();
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};

class GfxCmd_DebugLayer : public GraphicCommandBase
{
public:
	GfxCmd_DebugLayer();
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};

class GfxCmd_DrawDebugPrimitive : public GraphicCommandBase
{
private:
	Debug::DebugPrimitive myPrimitive;
	Matrix myTransform;
public:
	GfxCmd_DrawDebugPrimitive(Debug::DebugPrimitive primitive, Matrix Transform);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};

class GfxCmd_RenderMesh : public GraphicCommandBase
{
protected:
	std::vector< Element> myElementsData;
	std::vector<std::weak_ptr<Material>> myMaterials;
	Matrix myTransform;
	CU::Vector3<float> MinExtents; // 12 bytes 
	CU::Vector3<float> MaxExtents; // 12 bytes  

public:
	GfxCmd_RenderMesh(const RenderData& aMesh, const Matrix& aTransform);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};

class GfxCmd_RenderMeshShadow : public GfxCmd_RenderMesh
{
public:
	GfxCmd_RenderMeshShadow(const RenderData& aMesh, const Matrix& aTransform);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
class GfxCmd_RenderSkeletalMesh : public GfxCmd_RenderMesh
{
protected:
	Matrix myBoneTransforms[128];
public:
	GfxCmd_RenderSkeletalMesh(const RenderData& aMesh, const Matrix& aTransform, const Matrix* aBoneTransformList, unsigned int aNumBones);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};

class GfxCmd_RenderSkeletalMeshShadow : public GfxCmd_RenderSkeletalMesh
{
public:
	GfxCmd_RenderSkeletalMeshShadow(const RenderData& aMesh, const Matrix& aTransform, const Matrix* aBoneTransformList, unsigned int aNumBones);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};

class GfxCmd_RenderSkybox : public GraphicCommandBase
{
private:
	std::shared_ptr<Texture> mySkyboxTexture;
public:
	GfxCmd_RenderSkybox(std::shared_ptr<Texture> texture);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};