#pragma once

#include "Object/Object.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	template<IsObject InObjectType, bool bObjectValidation = false>
	class TObjectSharedPtr;

	template<IsObject InObjectType>
	class TObjectSharedPtr<InObjectType, true>
	{
	public:
		using ElementType = InObjectType;

	private:
		ElementType* Object = nullptr;

		struct FControlBlock
		{
			uint32_t RefCount;
			bool bObjectValid;
		}*ControlBlock = nullptr;

	public:
		TObjectSharedPtr() {}

		FORCEINLINE TObjectSharedPtr(const TObjectSharedPtr& Other)
			: Object(Other.Object)
			, ControlBlock(Other.ControlBlock)
		{
			++ControlBlock->RefCount;
		}

		FORCEINLINE TObjectSharedPtr(TObjectSharedPtr&& Other) noexcept
			: Object(std::exchange(Other.Object, nullptr))
			, ControlBlock(std::exchange(Other.ControlBlock, nullptr))
		{
		}

		FORCEINLINE TObjectSharedPtr(ElementType* const InObject)
			: Object(InObject)
			, ControlBlock(new FControlBlock(1, true))
		{
		}

		FORCEINLINE TObjectSharedPtr& operator=(const TObjectSharedPtr& Other)
		{
			if (Object && --ControlBlock->RefCount == 0)
			{
				if (ControlBlock->bObjectValid)
				{
					DeleteObject(Object);
				}
				delete ControlBlock;
			}

			Object = Other.Object;
			ControlBlock = Other.ControlBlock;

			++ControlBlock->RefCount;

			return *this;
		}

		FORCEINLINE TObjectSharedPtr& operator=(TObjectSharedPtr&& Other) noexcept
		{
			if (Object && --ControlBlock->RefCount == 0)
			{
				if (ControlBlock->bObjectValid)
				{
					DeleteObject(Object);
				}
				delete ControlBlock;
			}

			Object = std::exchange(Other.Object, nullptr);
			ControlBlock = std::exchange(Other.ControlBlock, nullptr);

			return *this;
		}

		FORCEINLINE TObjectSharedPtr& operator=(ElementType* const InObject)
		{
			if (Object && --ControlBlock->RefCount == 0)
			{
				if (ControlBlock->bObjectValid)
				{
					DeleteObject(Object);
				}
				delete ControlBlock;
			}

			Object = InObject;
			ControlBlock = new FControlBlock(1, true);

			return *this;
		}

		FORCEINLINE ~TObjectSharedPtr()
		{
			if (Object && --ControlBlock->RefCount == 0)
			{
				if (ControlBlock->bObjectValid)
				{
					DeleteObject(Object);
				}
				delete ControlBlock;
			}
		}

		NODISCARD FORCEINLINE ElementType* operator->()
		{
			return Object;
		}

		NODISCARD FORCEINLINE const ElementType* operator->() const
		{
			return Object;
		}

		NODISCARD FORCEINLINE ElementType* operator*()
		{
			return Object;
		}

		NODISCARD FORCEINLINE const ElementType* operator*() const
		{
			return Object;
		}

		NODISCARD FORCEINLINE bool IsValid() const
		{
			return ControlBlock;
		}

		NODISCARD FORCEINLINE bool IsObjectValid() const
		{
			return Object && ControlBlock->bObjectValid;
		}

		NODISCARD FORCEINLINE ElementType* GetObject()
		{
			return Object;
		}

		NODISCARD FORCEINLINE const ElementType* GetObject() const
		{
			return Object;
		}

		FORCEINLINE void MarkObjectAsValid()
		{
			BS_CHECK(Object);
			ControlBlock->bObjectValid = true;
		}

		FORCEINLINE void MarkObjectAsInvalid()
		{
			BS_CHECK(Object);
			ControlBlock->bObjectValid = false;
		}
	};

	template<IsObject InObjectType>
	class TObjectSharedPtr<InObjectType, false>
	{
	public:
		using ElementType = InObjectType;

	private:
		ElementType* Object = nullptr;
		uint32_t* RefCount = nullptr;

	public:
		FORCEINLINE TObjectSharedPtr() {}

		FORCEINLINE TObjectSharedPtr(const TObjectSharedPtr& Other)
			: Object(Other.Object)
			, RefCount(Other.RefCount)
		{
			++(*RefCount);
		}

		FORCEINLINE TObjectSharedPtr(TObjectSharedPtr&& Other) noexcept
			: Object(std::exchange(Other.Object, nullptr))
			, RefCount(std::exchange(Other.RefCount, nullptr))
		{
		}

		FORCEINLINE TObjectSharedPtr(ElementType* const InObject)
			: Object(InObject)
			, RefCount(new uint32_t(1))
		{
		}

		FORCEINLINE TObjectSharedPtr& operator=(const TObjectSharedPtr& Other)
		{
			if (Object && --(*RefCount) == 0)
			{
				DeleteObject(Object);
				delete RefCount;
			}

			Object = Other.Object;
			RefCount = Other.RefCount;

			++(*RefCount);

			return *this;
		}

		FORCEINLINE TObjectSharedPtr& operator=(TObjectSharedPtr&& Other) noexcept
		{
			if (Object && --(*RefCount) == 0)
			{
				DeleteObject(Object);
				delete RefCount;
			}

			Object = std::exchange(Other.Object, nullptr);
			RefCount = std::exchange(Other.RefCount, nullptr);

			return *this;
		}

		FORCEINLINE ~TObjectSharedPtr()
		{
			if (Object && --(*RefCount) == 0)
			{
				DeleteObject(Object);
				delete RefCount;
			}
		}

		NODISCARD FORCEINLINE ElementType* operator->()
		{
			return Object;
		}

		NODISCARD FORCEINLINE const ElementType* operator->() const
		{
			return Object;
		}

		NODISCARD FORCEINLINE ElementType* operator*()
		{
			return Object;
		}

		NODISCARD FORCEINLINE const ElementType* operator*() const
		{
			return Object;
		}

		NODISCARD FORCEINLINE bool IsValid() const
		{
			return RefCount;
		}

		NODISCARD FORCEINLINE ElementType* GetObject()
		{
			return Object;
		}

		NODISCARD FORCEINLINE const ElementType* GetObject() const
		{
			return Object;
		}
	};
}
