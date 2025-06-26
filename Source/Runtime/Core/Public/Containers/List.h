#pragma once

#include "Allocators/Allocator.h"
#include "Allocators/AllocatorTraits.h"
#include "Allocators/PoolAllocator.h"
#include "Misc/AssertionMacros.h"

namespace Bloodshot
{
	template<typename InElementType, typename InAllocatorType = TPoolAllocator<64>>
	class TList;

	template<typename InElementType>
	class TListNode
	{
		template<typename InElementType, typename InAllocatorType>
		friend class TList;

	public:
		using ElementType = InElementType;

		FORCEINLINE TListNode(const ElementType& InElement)
			: Element(InElement)
		{
		}

		FORCEINLINE TListNode(ElementType&& InElement)
			: Element(std::move(InElement))
		{
		}

		template<typename... ArgTypes>
		TListNode(ArgTypes&&... Args)
			: Element(std::forward<ArgTypes>(Args)...)
		{
		}

		~TListNode() {}

		NODISCARD FORCEINLINE ElementType& GetValue()
		{
			return Element;
		}

		NODISCARD FORCEINLINE const ElementType& GetValue() const
		{
			return Element;
		}

		NODISCARD FORCEINLINE TListNode* GetNextNode()
		{
			return NextNode;
		}

		NODISCARD FORCEINLINE const TListNode* GetNextNode() const
		{
			return NextNode;
		}

		NODISCARD FORCEINLINE TListNode* GetPreviousNode()
		{
			return PreviousNode;
		}

		NODISCARD FORCEINLINE const TListNode* GetPreviousNode() const
		{
			return PreviousNode;
		}

	private:
		ElementType Element;
		TListNode* NextNode = nullptr;
		TListNode* PreviousNode = nullptr;
	};

	template<typename InElementType>
	class TListIterator
	{
	public:
		using ElementType = InElementType;
		using NodeType = TListNode<ElementType>;

		explicit TListIterator(NodeType* const InitialNode)
			: CurrentNode(InitialNode)
		{
		}

		FORCEINLINE explicit operator bool() const
		{
			return CurrentNode;
		}

		FORCEINLINE bool operator==(const TListIterator& Right) const
		{
			return CurrentNode == Right.CurrentNode;
		}

		FORCEINLINE bool operator!=(const TListIterator& Right) const
		{
			return CurrentNode != Right.CurrentNode;
		}

		FORCEINLINE ElementType* operator->() const
		{
			BS_CHECK(CurrentNode);
			return &CurrentNode->GetValue();
		}

		FORCEINLINE ElementType& operator*() const
		{
			BS_CHECK(CurrentNode);
			return CurrentNode->GetValue();
		}

		FORCEINLINE TListIterator& operator++()
		{
			BS_CHECK(CurrentNode);
			CurrentNode = CurrentNode->GetNextNode();
			return *this;
		}

		FORCEINLINE TListIterator& operator++(int)
		{
			TListIterator Temp = *this;
			++(*this);
			return Temp;
		}

		FORCEINLINE TListIterator& operator--()
		{
			BS_CHECK(CurrentNode);
			CurrentNode = CurrentNode->GetPreviousNode();
			return *this;
		}

		FORCEINLINE TListIterator& operator--(int)
		{
			TListIterator Temp = *this;
			--(*this);
			return Temp;
		}

		NODISCARD FORCEINLINE NodeType* GetNode() const
		{
			BS_CHECK(CurrentNode);
			return CurrentNode;
		}

	private:
		NodeType* CurrentNode;
	};

	template<typename InElementType, typename InAllocatorType>
	class TList
	{
	public:
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;
		using ElementAllocatorType = typename TAllocatorTraits<AllocatorType>::template ForElementType<InElementType>;
		using NodeType = TListNode<ElementType>;
		using NodeAllocatorType = typename TAllocatorTraits<AllocatorType>::template ForElementType<NodeType>;
		using FIterator = TListIterator<ElementType>;
		using FConstIterator = TListIterator<const ElementType>;

		static_assert(TAllocatorTraits<AllocatorType>::IsNonContiguous,
			"TList requires non contiguous memory allocator");

	private:
		NodeAllocatorType Allocator;
		NodeType* Head = nullptr;
		NodeType* Tail = nullptr;
		size_t Size = 0;

	public:
		TList() {}

		FORCEINLINE ~TList()
		{
			Dispose();
		}

		NODISCARD FORCEINLINE NodeAllocatorType& GetAllocator()
		{
			return Allocator;
		}

		NODISCARD FORCEINLINE const NodeAllocatorType& GetAllocator() const
		{
			return Allocator;
		}

		NODISCARD FORCEINLINE size_t GetSize() const
		{
			return Size;
		}

		NODISCARD FORCEINLINE bool IsEmpty() const
		{
			return !Size;
		}

		NODISCARD FORCEINLINE NodeType* GetHead() const
		{
			return Head;
		}

		NODISCARD FORCEINLINE NodeType* GetTail() const
		{
			return Tail;
		}

		NODISCARD bool AddHead(NodeType* const NewNode)
		{
			if (!NewNode)
			{
				return false;
			}

			if (Head)
			{
				NewNode->NextNode = Head;
				Head->PreviousNode = NewNode;
				Head = NewNode;
			}
			else
			{
				Head = NewNode;
				Tail = NewNode;
			}

			++Size;
			return true;
		}

		FORCEINLINE bool AddHead(const ElementType& InElement)
		{
			NodeType* const NewNode = Allocator.Allocate();
			new(NewNode) NodeType(InElement);
			return AddHead(NewNode);
		}

		FORCEINLINE bool AddHead(ElementType&& InElement)
		{
			NodeType* const NewNode = Allocator.Allocate();
			new(NewNode) NodeType(std::move(InElement));
			return AddHead(NewNode);
		}

		template<typename... ArgTypes>
		FORCEINLINE bool AddHead(ArgTypes&&... Args)
		{
			NodeType* const NewNode = Allocator.Allocate();
			new(NewNode) NodeType(std::forward<ArgTypes>(Args)...);
			return AddHead(NewNode);
		}

		NODISCARD bool AddTail(NodeType* const NewNode)
		{
			if (!NewNode)
			{
				return false;
			}

			if (Tail)
			{
				Tail->NextNode = NewNode;
				NewNode->PreviousNode = Tail;
				Tail = NewNode;
			}
			else
			{
				Head = NewNode;
				Tail = NewNode;
			}

			++Size;
			return true;
		}

		FORCEINLINE bool AddTail(const ElementType& InElement)
		{
			NodeType* const NewNode = (NodeType*)Allocator.Allocate();
			new(NewNode) NodeType(InElement);
			return AddTail(NewNode);
		}

		FORCEINLINE bool AddTail(ElementType&& InElement)
		{
			NodeType* const NewNode = (NodeType*)Allocator.Allocate();
			new(NewNode) NodeType(std::move(InElement));
			return AddTail(NewNode);
		}

		template<typename... ArgTypes>
		FORCEINLINE bool AddTail(ArgTypes&&... Args)
		{
			NodeType* const NewNode = (NodeType*)Allocator.Allocate();
			new(NewNode) NodeType(std::forward<ArgTypes>(Args)...);
			return AddTail(NewNode);
		}

		NODISCARD bool Insert(NodeType* const NewNode, NodeType* const NodeToInsertBefore = nullptr)
		{
			if (!NewNode)
			{
				return false;
			}

			if (!NodeToInsertBefore || NodeToInsertBefore == Head)
			{
				return AddHead(NewNode);
			}

			NewNode->PreviousNode = NodeToInsertBefore->PreviousNode;
			NewNode->NextNode = NodeToInsertBefore;

			NodeToInsertBefore->PreviousNode->NextNode = NewNode;
			NodeToInsertBefore->PreviousNode = NewNode;

			++Size;
			return true;
		}

		FORCEINLINE bool Insert(const ElementType& InElement, NodeType* const NodeToInsertBefore = nullptr)
		{
			NodeType* const NewNode = Allocator.Allocate();
			new(NewNode) NodeType(InElement);
			return Insert(NewNode, NodeToInsertBefore);
		}

		FORCEINLINE bool Insert(ElementType&& InElement, NodeType* const NodeToInsertBefore = nullptr)
		{
			NodeType* const NewNode = Allocator.Allocate();
			new(NewNode) NodeType(std::move(InElement));
			return Insert(NewNode, NodeToInsertBefore);
		}

		template<typename... ArgTypes>
		FORCEINLINE bool Insert(NodeType* const NodeToInsertBefore, ArgTypes&&... Args)
		{
			NodeType* const NewNode = Allocator.Allocate();
			new(NewNode) NodeType(std::forward<ArgTypes>(Args)...);
			return Insert(NewNode, NodeToInsertBefore);
		}

		NODISCARD FORCEINLINE NodeType* Find(const ElementType& InElement)
		{
			NodeType* CurrentNode = Head;
			while (CurrentNode)
			{
				if (CurrentNode->GetValue() == InElement)
				{
					break;
				}
				CurrentNode = CurrentNode->NextNode;
			}
			return CurrentNode;
		}

		NODISCARD FORCEINLINE const NodeType* Find(const ElementType& InElement) const
		{
			return const_cast<TList*>(this)->Find(InElement);
		}

		template<typename InPredicateType>
		NODISCARD FORCEINLINE NodeType* FindByPredicate(InPredicateType InPredicate)
		{
			NodeType* CurrentNode = Head;
			while (CurrentNode)
			{
				if (InPredicate(CurrentNode->GetValue()))
				{
					break;
				}
				CurrentNode = CurrentNode->NextNode;
			}
			return CurrentNode;
		}

		template<typename InPredicateType>
		NODISCARD FORCEINLINE const NodeType* FindByPredicate(InPredicateType InPredicate) const
		{
			return const_cast<TList*>(this)->FindByPredicate(InPredicate);
		}

		NODISCARD FORCEINLINE bool Contains(const ElementType& InElement) const
		{
			return Find(InElement);
		}

		void Remove(NodeType* InNode)
		{
			if (!InNode)
			{
				return;
			}

			if (Size == 1)
			{
				BS_CHECK(InNode == Head);
				Clear();
				return;
			}

			if (InNode == Head)
			{
				Head = Head->NextNode;
				Head->PreviousNode = nullptr;
			}
			else if (InNode == Tail)
			{
				Tail = Tail->PreviousNode;
				Tail->NextNode = nullptr;
			}
			else
			{
				InNode->NextNode->PreviousNode = InNode->PreviousNode;
				InNode->PreviousNode->NextNode = InNode->NextNode;
			}

			DestructElement(InNode);
			Allocator.Deallocate(InNode);
			--Size;
		}

		FORCEINLINE void Remove(const ElementType& InElement)
		{
			NodeType* ExistingNode = Find(InElement);
			Remove(ExistingNode);
		}

		template<typename InPredicateType>
		FORCEINLINE void RemoveByPredicate(InPredicateType InPredicate)
		{
			NodeType* Found = FindByPredicate(InPredicate);
			Remove(Found);
		}

		FORCEINLINE void Clear()
		{
			NodeType* CurrentNode;
			while (Head)
			{
				CurrentNode = Head->NextNode;
				DestructElement(Head);
				Head = CurrentNode;
			}

			Allocator.Reset();
			Head = nullptr;
			Tail = nullptr;
			Size = 0;
		}

		FORCEINLINE void Dispose()
		{
			NodeType* CurrentNode;
			while (Head)
			{
				CurrentNode = Head->NextNode;
				DestructElement(Head);
				Head = CurrentNode;
			}

			Allocator.Dispose();
			Head = nullptr;
			Tail = nullptr;
			Size = 0;
		}

		// For internal usage only!
		FORCEINLINE FIterator begin() { return FIterator(Head); }
		FORCEINLINE FIterator end() { return FIterator(nullptr); }
		FORCEINLINE FConstIterator begin() const { return FConstIterator(Head); }
		FORCEINLINE FConstIterator end() const { return FConstIterator(nullptr); }
	};
}
