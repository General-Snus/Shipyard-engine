#pragma once



class Mesh;
class TextureHolder;
class Material;



class GraphicsEngineUtilities
{
public:
	//Not thread safe
	static bool GenerateSceneForIcon(std::shared_ptr<Mesh> meshAsset,std::shared_ptr<TextureHolder> renderTarget,std::shared_ptr<Material> material);
};

