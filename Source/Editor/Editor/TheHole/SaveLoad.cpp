
#ifdef MOTHBALLED


template<typename T>
struct SaveData
{
	int fnc;
	std::string identifier;
	T* arg;
};
enum class eSaveToJsonArgument
{
	InputFloat3,
	InputFloat4,
	SaveBool
};

bool SaveDataToJson() const;
bool JsonToSaveData() const;
bool ContainData(SaveData<float>& data);
bool SaveToMemory(eSaveToJsonArgument fnc,const std::string& identifier,void* arg);
bool SaveToMemory(SaveData<float>& arg);
std::vector<SaveData<float>> mySaveData;



bool Editor::SaveDataToJson() const
{
	std::string path = "myJson.json";
	std::ofstream o(path);

	nlohmann::json j;
	for(auto& i : GameObjectManager::GetInstance().GetAllComponents<BackgroundColor>())
	{
		j.push_back((json)i);
	}

	o << std::setw(4) << j << std::endl;

	return true;
}
bool Editor::JsonToSaveData() const
{
	std::string path = "myJson.json";
	if(!std::filesystem::exists(path))
	{
		return false;
	}
	std::ifstream i(path);
	assert(i.is_open());
	nlohmann::json json = nlohmann::json::parse(i);
	i.close();
	GameObjectManager& gom = GameObjectManager::GetInstance();
	GameObject newG = gom.CreateGameObject();

	for(nlohmann::json& components : json)
	{
		if(components["myComponentType"] == eComponentType::backgroundColor)
		{
			newG.AddComponent < BackgroundColor>();
			newG.GetComponent<BackgroundColor>().SetColor(
				{
				components["myColor.x"],
				components["myColor.y"],
				components["myColor.z"],
				components["myColor.w"]
				});
		}
	}
	return true;
}
bool Editor::ContainData(SaveData<float>& data)
{
	for(SaveData<float>& i : mySaveData)
	{
		if(i.fnc == data.fnc && i.identifier == data.identifier)
		{
			i.arg = data.arg;
			return true;
		}
	}
	return false;
}
bool Editor::SaveToMemory(eSaveToJsonArgument fnc,const std::string& identifier,void* arg)
{
	switch(fnc)
	{
	case eSaveToJsonArgument::InputFloat3:
	{
		const auto* x = (float*)arg;
		arg = (float*)arg + 1;
		const auto* y = (float*)arg;
		arg = (float*)arg + 1;
		const auto* z = (float*)arg;

		for(SaveData<float>& i : mySaveData)
		{
			if(i.fnc == (int)fnc && i.identifier == identifier)
			{
				i.arg[0] = *x;
				i.arg[1] = *y;
				i.arg[2] = *z;
				return true;
			}
		}

		SaveData<float> data;
		data.fnc = (int)fnc;
		data.identifier = identifier;
		data.arg = new float[3];
		data.arg[0] = (*x);
		data.arg[1] = (*y);
		data.arg[2] = (*z);
		mySaveData.push_back(data);
		return true;
	}
	case eSaveToJsonArgument::SaveBool:
	{
		auto* x = (bool*)arg;
		for(SaveData<float>& i : mySaveData)
		{
			if(i.fnc == (int)fnc && i.identifier == identifier)
			{
				i.arg[0] = *x;
				return true;
			}
		}

		SaveData<float> data;
		data.fnc = (int)fnc;
		data.identifier = identifier;
		data.arg = new float[1];
		data.arg[0] = (*x);
		mySaveData.push_back(data);
		return true;
	}
	default:
		std::cout << "SaveFunction can not handle this argument";
		return false;
		break;
	}
}
bool Editor::SaveToMemory(SaveData<float>& arg)
{
	for(SaveData<float>& i : mySaveData)
	{
		if(i.fnc == arg.fnc && i.identifier == arg.identifier)
		{
			i.arg = arg.arg;
			return true;
		}
	}
	mySaveData.push_back(arg);
	return true;
}

#endif // MOTHBALLED