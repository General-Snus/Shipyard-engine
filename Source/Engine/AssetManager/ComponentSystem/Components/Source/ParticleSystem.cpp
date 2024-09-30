#include "Engine/AssetManager/AssetManager.pch.h"
//#include "../ParticleSystem.h"
//#include <Tools/Utilities/Game/Timer.h>
//
//ParticleSystem::ParticleSystem(const unsigned int anOwnerId) : Component(anOwnerId)
//{
//}
//
//ParticleSystem::ParticleSystem(const unsigned int anOwnerId,const std::filesystem::path& aFilePath) : Component(anOwnerId)
//{
//	aFilePath;
//	//myEmitters.reserve(1);
//	//EngineResourceLoader::GetInstance().LoadAsset<ParticleEmitter>(aFilePath,myEmitters[0]);
//}
//
//void ParticleSystem::Draw()
//{
//	OPTICK_EVENT();
//	GraphicsEngineInstance.SetDepthState(GraphicsEngine::eDepthStencilStates::DSS_ReadOnly);
//	Transform* trns = TryGetComponent<Transform>();
//	if(trns)
//	{
//		ObjectBuffer& objectBuffer = GraphicsEngineInstance.myObjectBuffer;
//		objectBuffer.Data.myTransform = trns->GetTransform();
//		RHI::UpdateConstantBufferData(objectBuffer);
//	}
//	for(auto& i : myEmitters)
//	{
//		i->Draw();
//	}
//	GraphicsEngineInstance.SetDepthState(GraphicsEngine::eDepthStencilStates::DSS_ReadWrite);
//}
//
//void ParticleSystem::AddEmitter(const ParticleEmitterTemplate& aTemplate)
//{
//	myEmitters.push_back(std::make_shared<ParticleEmitter>(aTemplate));
//}
//void ParticleSystem::Update()
//{
//	OPTICK_EVENT();
//	for(auto& i : myEmitters)
//	{
//		i->Update(TimerInstance.GetInstance().GetDeltaTime());
//	}
//}