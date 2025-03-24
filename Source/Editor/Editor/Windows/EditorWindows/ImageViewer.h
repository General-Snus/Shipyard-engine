#pragma once 
#include "Editor/Editor/Windows/EditorWindows/EditorWindow.h"

class Texture;
class TextureHolder;
class ImageViewer : public EditorWindow {
public:
	explicit ImageViewer(std::shared_ptr<Texture> aTexture);
	explicit ImageViewer(std::shared_ptr<TextureHolder> aTexture);
	void RenderImGUi() override;
private:
	std::shared_ptr<Texture> texture;
};

