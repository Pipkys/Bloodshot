#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) // Variable '(UniqueID, TypeID, ObjectClass)' is uninitialized

#include "Object/ObjectMacros.h"
#include "Platform/Platform.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	class IObject;

	template<typename T>
	concept IsObject = std::is_base_of_v<IObject, T>;

	namespace Private::Object
	{
		class FObjectCore;

		template<typename ClassType, typename FunctionType, IsObject ObjectType, typename... ArgTypes>
		class IFunctionCaller;
	}

	class FClass;

	BSCLASS();
	class IObject
	{
		friend class Private::Object::FObjectCore;

	public:
		IObject() {}
		IObject(const IObject&) = delete;
		IObject(IObject&&) = delete;
		IObject& operator=(const IObject&) = delete;
		IObject& operator=(IObject&&) = delete;
		virtual ~IObject() {}

		inline static FClass* StaticClass()
		{
			return GetPrivateStaticClass();
		};

		NODISCARD FORCEINLINE uint32_t GetUniqueID() const
		{
			return UniqueID;
		}

		NODISCARD FORCEINLINE uint32_t GetTypeID() const
		{
			return TypeID;
		}

		NODISCARD FORCEINLINE IObject* GetObject()
		{
			return this;
		}

		NODISCARD FORCEINLINE const IObject* GetObject() const
		{
			return this;
		}

		NODISCARD FORCEINLINE FClass* GetClass() const
		{
			return ObjectClass;
		}

		NODISCARD bool IsA(const FClass* const InClass) const;

	private:
		uint32_t UniqueID;
		uint32_t TypeID;
		FClass* ObjectClass;

		static FClass* GetPrivateStaticClass();
	};
}

#pragma warning(pop)
