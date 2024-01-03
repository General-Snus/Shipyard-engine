#pragma once
#include <Tools/Utilities/System/SingletonTemplate.h>  
#include "ScriptGraphEditor/ScriptGraphEditor.h"
namespace Graph {
	class GraphTool : public Singleton<GraphTool> {
	public:
		friend class Singleton<GraphTool>;

		std::shared_ptr<ScriptGraphEditor> GetScriptingEditor();

	private:
		GraphTool();
		~GraphTool();
	};
}
