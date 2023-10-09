#pragma once
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp>

#include <GraphicsEngine/Rendering/Buffers/FrameBuffer.h>
#include <GraphicsEngine/Rendering/Buffers/ObjectBuffer.h> 
#include <GraphicsEngine/Rendering/Buffers/LightBuffer.h> 
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h> 
#include <GraphicsEngine/Rendering/Buffers/LineBuffer.h> 
#include <GraphicsEngine/Rendering/Buffers/G_buffer.h> 

#include <AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>

typedef CU::Matrix4x4<float>  Matrix;

class GraphicCommand
{
protected:
	FrameBuffer& GetFrameBuffer();
	ObjectBuffer& GetObjectBuffer();
	LineBuffer& GetLineBuffer();
	LightBuffer& GetLightBuffer();
	G_Buffer& GetGBuffer();
public:
	virtual ~GraphicCommand() = default;
	virtual void Execute() = 0;
};

class GfxCmd_SetFrameBuffer : public GraphicCommand
{
private:
	Matrix myViewMatrix;
	Matrix myProjectionMatrix;
	Vector3f myPosition;
	float myDeltaTime;
	 int RenderMode;
public:
	GfxCmd_SetFrameBuffer( const Matrix& ProjectionMatrix,const Transform& ref,int aRenderMode);
	GfxCmd_SetFrameBuffer( const Matrix& ProjectionMatrix,const Matrix& ref,int aRenderMode);
	void Execute() override;
};

class GfxCmd_SetLightBuffer : public GraphicCommand
{ 
	std::vector<std::pair< DirectionalLight*,Texture*>> dirLight;
	std::vector<std::pair< PointLight*,Texture*>> pointLight;
	std::vector<std::pair< SpotLight*,Texture*>> spotLight; 
public:
	GfxCmd_SetLightBuffer( );
	void Execute() override;
}; 

class GfxCmd_DebugLayer : public GraphicCommand
{
public:
	GfxCmd_DebugLayer();
	void Execute() override;
};

class GfxCmd_DrawDebugPrimitive : public GraphicCommand
{ 
private:
	Debug::DebugPrimitive myPrimitive; 
	Matrix myTransform;
public:
	GfxCmd_DrawDebugPrimitive(Debug::DebugPrimitive primitive,Matrix Transform);
	void Execute() override;
};

class GfxCmd_RenderMesh : public GraphicCommand
{
protected: 
	std::vector< Element> myElementsData;
	std::vector<std::weak_ptr<Material>> myMaterials;
	Matrix myTransform;
	CU::Vector3<float> MinExtents; // 12 bytes 
	CU::Vector3<float> MaxExtents; // 12 bytes  

public:
	GfxCmd_RenderMesh(const RenderData& aMesh, const Matrix& aTransform);
	void Execute() override;
};

class GfxCmd_RenderMeshShadow : public GfxCmd_RenderMesh
{  
public:
	GfxCmd_RenderMeshShadow(const RenderData& aMesh, const Matrix& aTransform);
	void Execute() override;
};
class GfxCmd_RenderSkeletalMesh : public GfxCmd_RenderMesh
{
protected:
	Matrix myBoneTransforms[128];
public:
	GfxCmd_RenderSkeletalMesh(const RenderData& aMesh,const Matrix& aTransform,const Matrix* aBoneTransformList,unsigned int aNumBones);
	void Execute() override;
};

class GfxCmd_RenderSkeletalMeshShadow : public GfxCmd_RenderSkeletalMesh
{
public:
	GfxCmd_RenderSkeletalMeshShadow(const RenderData& aMesh,const Matrix& aTransform,const Matrix* aBoneTransformList,unsigned int aNumBones);
	void Execute() override;
};

class GfxCmd_RenderSkybox : public GraphicCommand
{
private:
	std::shared_ptr<Texture> mySkyboxTexture;
public:
	GfxCmd_RenderSkybox(std::shared_ptr<Texture> texture);
	void Execute() override;
};