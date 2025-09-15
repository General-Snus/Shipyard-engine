#pragma once
#include <memory>

class SelectionHandler;
class HandlerCollection
{
	friend class Editor;
public:
	//Getters 
	const std::shared_ptr<SelectionHandler> GetSelectionHandler() const { return selectionHandler; }

private:
	//Handlers
	std::shared_ptr<SelectionHandler> selectionHandler;


	void Init();
	void Update();
};

class Handler {
	friend class HandlerCollection;
protected:
	const HandlerCollection * collectionLink;

	virtual void Init() {};
	virtual void Update(){};
};
