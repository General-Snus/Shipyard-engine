//General Graphics settings
#ifndef Registry
#define Registry


#define defaultTextureFormat DXGI_FORMAT_R16G16B16A16_FLOAT
//DXGI_FORMAT_R32G32B32A32_FLOAT
//DXGI_FORMAT_R32G32B32A32_TYPELESS
//DXGI_FORMAT_R16G16B16A16_FLOAT 
//DXGI_FORMAT_R16G16B16A16_UNORM
//DXGI_FORMAT_R8G8B8A8_UNORM


// Purpose: Registers for the shaders.

#define HLSL_REG_DefaultSampler s10 
#define REG_DefaultSampler 10 

#define HLSL_REG_normalDepthSampler s11
#define REG_normalDepthSampler 11

#define HLSL_REG_PointSampler s12 
#define REG_PointSampler 12 

#define HLSL_REG_shadowCmpSampler s13 
#define REG_shadowCmpSampler 13 

#define HLSL_REG_BRDFSampler s14 
#define REG_BRDFSampler 14 

#define HLSL_REG_FrameBuffer b0 
#define REG_FrameBuffer 0

#define HLSL_REG_ObjectBuffer b1 
#define REG_ObjectBuffer 1 

#define HLSL_REG_DefaultMaterialBuffer b2 
#define REG_DefaultMaterialBuffer 2 

#define HLSL_REG_LightBuffer b3 
#define REG_LightBuffer 3 

#define HLSL_REG_LineBuffer b5 
#define REG_LineBuffer 5

#define HLSL_REG_GraphicSettingsBuffer b13 
#define REG_GraphicSettingsBuffer 13

#define HLSL_REG_colorMap t8 
#define REG_colorMap 8 

#define HLSL_REG_normalMap t1 
#define REG_normalMap 1 

#define HLSL_REG_materialMap t2 
#define REG_materialMap 2 

#define HLSL_REG_effectMap t3 
#define REG_effectMap 3  

#define HLSL_REG_VertexNormal t4 
#define REG_VertexNormal 4  

#define HLSL_REG_WorldPosition t5 
#define REG_WorldPosition 5  

#define HLSL_REG_DepthMap t6 
#define REG_DepthMap 6  

#define HLSL_REG_SSAO t7 
#define REG_SSAO 7

#define HLSL_REG_Target0 t10 
#define REG_Target0 10   

#define HLSL_REG_Target01 t11 
#define REG_Target01 11   

#define HLSL_REG_Target02 t12 
#define REG_Target02 12   

#define HLSL_REG_Target03 t13 
#define REG_Target03 13

#define HLSL_REG_Target04 t14 
#define REG_Target04 14

#define HLSL_REG_enviromentCube t100
#define REG_enviromentCube 100 

#define HLSL_REG_dirLightShadowMap t110 
#define REG_dirLightShadowMap 110 

#define HLSL_REG_Noise_Texture t126
#define REG_Noise_Texture 126  

#define HLSL_REG_BRDF_LUT_Texture t127
#define REG_BRDF_LUT_Texture 127

#endif
