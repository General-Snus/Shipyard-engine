#include "Collection.h" 
#include "SelectionHandler.h"

void HandlerCollection::Init() {
	//Do individually and explicitly, why? Because I said so.
	selectionHandler = std::make_shared<SelectionHandler>();
	selectionHandler->collectionLink = this;
	selectionHandler->Init();

}

void HandlerCollection::Update()
{
	selectionHandler->Update();
}
