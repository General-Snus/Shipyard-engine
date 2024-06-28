#pragma once
#include "EditorWindow.h"

class Inspector : public EditorWindow
{
	void RenderImGUi() override;
};

template <class F,std::size_t... Is>
void for_(F func,std::index_sequence<Is...>)
{
	(func(Is),...);
}
