//#pragma once 
//
//enum eCallbackAction
//{
//	loaded,unloaded,deleted,changed,renamed,created,count
//};
//
//class AssetCallbackMaster
//{
//	friend class AssetManager;
//public: 
//	struct Message
//	{
//		eCallbackAction event;
//		std::type_info* type;
//		const std::filesystem::path& path;
//	};
//private:
//	struct dataStruct
//	{
//		//std::function<void(Message)> callbacksFunction;
//		SY::UUID subscribed;
//	};
//
//private:
//	template<typename T>
//	void UpdateStatusOf(const std::filesystem::path& path,eCallbackAction event);
//	AssetCallbackMaster() = default;
//	//activate on path, send to function call universal, need uuid and send action
//	std::unordered_map<std::filesystem::path,std::vector<dataStruct>> callbacks; 
//};