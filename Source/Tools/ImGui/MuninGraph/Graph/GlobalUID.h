#pragma once
#include <string>
#include <typeinfo>
#include <regex>

namespace Munin
{
	class UID
	{
		static inline size_t nextBaseUID = 1;
		size_t myUID;
	public:

		// ctors get a new UID.
		UID()
			: myUID(nextBaseUID++)
		{  }

		// copies get their own UID.
		UID(const UID& other)
			: myUID(nextBaseUID++)
		{
			other;
		}

		// Moved objects retain their UID.
		UID(UID&& other) noexcept
			: myUID(other.myUID)
		{ }

		UID& operator=(const UID& other)
		{
			myUID = other.myUID;
			return *this;
		}

		UID& operator=(UID&& other) noexcept
		{
			std::swap(myUID, other.myUID);
			return *this;
		}

		__forceinline size_t GetUID() const { return myUID; }
	};

	/**
	 * \brief Base container for the actual UUID so that it's retrievable without
	 * knowing the owning type.
	 */
	class GlobalUID : public UID
	{
		std::string myUUID;
		const std::type_info& myRTTInfo;
		const std::string myOwnerTypeName;

		[[nodiscard]] std::string PopulateTypeName() const
		{
			const std::string MSVCTypeName = myRTTInfo.name();
			const size_t fromPos = MSVCTypeName.find_first_of(' ');

			size_t toPos = MSVCTypeName.find_first_of(' ', fromPos);
			if(toPos == std::string::npos)
			{
				toPos = MSVCTypeName.size() - fromPos;
			}
			else
			{
				toPos = MSVCTypeName.size() - toPos;
			}

			std::string result = MSVCTypeName.substr(fromPos + 1, toPos);

			result = std::regex_replace(result, std::regex(R"(((\bclass\b)|(\bstruct\b))\s*)"), "");

			return result;
		}

	protected:		

		GlobalUID(const std::type_info& aType)
			: myRTTInfo(aType), myOwnerTypeName(PopulateTypeName())
		{ }

		void SetUUID(const std::string& aNewUUID)
		{
			myUUID = aNewUUID;
		}

	public:

		GlobalUID()
			: myUUID("Invalid"), myRTTInfo(typeid(std::nullptr_t)), myOwnerTypeName("Invalid")
		{  }

		GlobalUID(const GlobalUID& other)
			: myUUID(other.myUUID), myRTTInfo(other.myRTTInfo), myOwnerTypeName(other.myOwnerTypeName)
		{	}

		GlobalUID(GlobalUID&& other) noexcept
			: UID(std::move(other)), myUUID(std::move(other.myUUID)), myRTTInfo(other.myRTTInfo), myOwnerTypeName(other.myOwnerTypeName)
		{	}

		__forceinline std::string GetUUID() const { return myUUID; }
		__forceinline std::string GetTypeName() const { return myOwnerTypeName; }
		__forceinline const std::type_info& GetRTTId() const { return myRTTInfo; }
	};

	// Quick macro to fetch the provided object as a Munin::GlobalUID pointer.
	#define AsGUIDAwarePtr(O) dynamic_cast<const Munin::GlobalUID*>(O)
	#define AsGUIDAwareSharedPtr(O) std::dynamic_pointer_cast<const Munin::GlobalUID>(O)

	/**
	 * \brief Represents a Globally Unique IDentifier based on the class name and the instance used.
	 * \tparam Class The class we should base the UUID on.
	 */
	template<typename Class>
	struct ObjectGUID : public GlobalUID
	{
	private:
		static inline size_t nextInstanceId = 0;
		//const std::type_info& myRTTInfo = typeid(Class);
		//const std::string myOwnerTypeName = PopulateTypeName();

	public:

		// Create a new GlobalUID based on our owning type.
		// The ## is to allow the name to be used in ImGui without showing up in text fields.
		ObjectGUID()
			: GlobalUID(typeid(Class))
		{
			SetUUID("##" + GetTypeName() + "_" + std::to_string(nextInstanceId++));
		}

		// Create a new GlobalUID based on our owning type.
		// The ## is to allow the name to be used in ImGui without showing up in text fields.
		ObjectGUID(const ObjectGUID& other)
			: GlobalUID(other)
		{
			SetUUID("##" + GetTypeName() + "_" + std::to_string(nextInstanceId++));
		}

		ObjectGUID(ObjectGUID&& other) noexcept
			: GlobalUID(std::move(other))
		{  }
	};
}

template<>
struct std::hash<Munin::UID>
{
	auto operator()(const Munin::UID& aUID) const noexcept -> size_t
	{
		return std::hash<size_t>{}(aUID.GetUID());
	}
};