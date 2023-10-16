// TgaFbxTestTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cassert>
#include <chrono>

#include "TgaFbx\Tgafbx.h"

std::wostream& PrintAtDepth(size_t aLevel)
{
	for(size_t l = 0; l < aLevel; l++)
	{
		std::wcout << "\t";
	}

	return std::wcout;
}

int main()
{
	std::wstring aFileName = L"Locator_Cube.fbx";

	TGA::FBX::Importer::InitImporter();

	//TGA::FBX::Mesh cubeMesh;
	//TGA::FBX::Importer::LoadMeshA("1MCube_MSettingMayaMScaleFactorExport.fbx", cubeMesh);

	TGA::FBX::Mesh tgaMesh;
	TGA::FBX::Importer::LoadMeshA("SK_HeavyEnemy.fbx", tgaMesh);

	TGA::FBX::Animation tgaAnimation;
	TGA::FBX::Importer::LoadAnimationA("Anim_Heavy_Statue.fbx", tgaAnimation);

	
	std::wcout << "Blopp" << std::endl;

	//TGA::FBX::Model aModel;
	//TGA::FBX::Importer::InitImporter();
	//TGA::FBX::Importer::LoadModel(aFileName, aModel);

	//std::wcout << "File Information for " << aFileName.c_str() << ":" << std::endl;
	//std::wcout << std::endl;

	//std::wcout << aModel.LODGroups.size() << " LOD Groups found." << std::endl;
	//std::wcout << aModel.Meshes.size() << " meshes without a LOD Group." << std::endl;
	//std::wcout << aModel.Materials.size() << " materials found." << std::endl;

	//std::wcout << std::endl;

	//size_t aPrintDepth = 0;

	//if(!aModel.LODGroups.empty())
	//{
	//	std::wcout << "LOD Groups:" << std::endl;
	//	aPrintDepth++;
	//	for(const TGA::FBX::Model::LODGroup& aLodGroup : aModel.LODGroups)
	//	{			
	//		for(size_t l = 0; l < aLodGroup.Levels.size(); l++)
	//		{
	//			PrintAtDepth(aPrintDepth) << "Level" << std::to_wstring(l) << ":" << std::endl;
	//			PrintAtDepth(aPrintDepth) << "Distance: " << aLodGroup.Levels[l].Distance << std::endl;
	//			aPrintDepth++;
	//			for(size_t m = 0; m < aLodGroup.Levels[l].Meshes.size(); m++)
	//			{
	//				//PrintAtDepth(aPrintDepth) << "Mesh: " << aLodGroup.Levels[l].Meshes[m].MeshName << std::to_wstring(l) << ":" << std::endl;
	//				//std::wcout << "Distance: " << aLodGroup.Levels[l].Meshes << std::endl;
	//			}				
	//		}			
	//	}
	//}


	//std::wcout << "Meshes:" << std::endl;

	//for(const TGA::FBX::Model::Mesh& aMesh : aModel.Meshes)
	//{
	//	


}