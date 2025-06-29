// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <atomic>
#include <optional>
#include <cstdlib>
#include "template.h"

/**
 * Fast single-producer/single-consumer unbounded concurrent queue. Doesn't free memory until destruction but recycles consumed items.
 * Based on http://www.1024cores.net/home/lock-free-algorithms/queues/unbounded-spsc-queue
 */
template<typename T>
class TSpscQueue final
{
	using ElementType = T;

	void DestructElement(ElementType* Element) {
		if constexpr (!std::is_trivially_destructible_v<ElementType>)
		{
			Element->ElementType::~ElementType();
		}
	}

public:

	TSpscQueue(const TSpscQueue&) = delete;
	TSpscQueue(TSpscQueue&&) = delete;
	const TSpscQueue& operator=(const TSpscQueue&) = delete;
	const TSpscQueue& operator=(TSpscQueue&&) = delete;

	TSpscQueue()
	{
		
		FNode* Node = ::new(std::malloc(sizeof(FNode))) FNode;
		Tail.store(Node, std::memory_order_relaxed);
		Head = First = TailCopy = Node;
	}

	~TSpscQueue()
	{
		FNode* Node = First;
		FNode* LocalTail = Tail.load(std::memory_order_relaxed);

		// Delete all nodes which are the sentinel or unoccupied
		bool bContinue = false;
		do
		{
			FNode* Next = Node->Next.load(std::memory_order_relaxed);
			bContinue = Node != LocalTail;
			std::free(Node);
			Node = Next;
		} while (bContinue);

		// Delete all nodes which are occupied, destroying the element first
		while (Node != nullptr)
		{
			FNode* Next = Node->Next.load(std::memory_order_relaxed);
			DestructElement((ElementType*)&Node->Value);
			std::free(Node);
			Node = Next;
		}
	}

	template <typename... ArgTypes>
	void Enqueue(ArgTypes&&... Args)
	{
		FNode* Node = AllocNode();
		::new((void*)&Node->Value) ElementType(std::forward<ArgTypes>(Args)...);

		Head->Next.store(Node, std::memory_order_release);
		Head = Node;
	}

	// returns empty TOptional if queue is empty 
	std::optional<ElementType> Dequeue()
	{
		FNode* LocalTail = Tail.load(std::memory_order_relaxed);
		FNode* LocalTailNext = LocalTail->Next.load(std::memory_order_acquire);
		if (LocalTailNext == nullptr)
		{
			return {};
		}

		ElementType* TailNextValue = (ElementType*)&LocalTailNext->Value;
		std::optional<ElementType> Value{ std::move(*TailNextValue) };
		DestructElement(TailNextValue);

		Tail.store(LocalTailNext, std::memory_order_release);
		return Value;
	}

	bool Dequeue(ElementType& OutElem)
	{
		std::optional<ElementType> LocalElement = Dequeue();
		if (LocalElement.has_value())
		{
			OutElem = MoveTempIfPossible(LocalElement.GetValue());
			return true;
		}
		
		return false;
	}

	bool IsEmpty() const
	{
		FNode* LocalTail = Tail.load(std::memory_order_relaxed);
		FNode* LocalTailNext = LocalTail->Next.load(std::memory_order_acquire);
		return LocalTailNext == nullptr;
	}

	// as there can be only one consumer, a consumer can safely "peek" the tail of the queue.
	// returns a pointer to the tail if the queue is not empty, nullptr otherwise
	// there's no overload with TOptional as it doesn't support references
	ElementType* Peek() const
	{
		FNode* LocalTail = Tail.load(std::memory_order_relaxed);
		FNode* LocalTailNext = LocalTail->Next.load(std::memory_order_acquire);

		if (LocalTailNext == nullptr)
		{
			return nullptr;
		}

		return (ElementType*)&LocalTailNext->Value;
	}

private:
//#pragma warning(push)
//#pragma warning(disable: 4324) // Disable warning 4996 (example)
//	struct alignas(std::hardware_destructive_interference_size) FNode
//#pragma warning(pop)
	struct FNode
	{
		std::atomic<FNode*> Next{ nullptr };
		TTypeCompatibleBytes<ElementType> Value;
	};

private:
	FNode* AllocNode()
	{
		// first tries to allocate node from internal node cache, 
		// if attempt fails, allocates node via ::operator new() 

		auto AllocFromCache = [this]()
		{
			FNode* Node = First;
			First = First->Next;
			Node->Next.store(nullptr, std::memory_order_relaxed);
			return Node;
		};

		if (First != TailCopy)
		{
			return AllocFromCache();
		}

		TailCopy = Tail.load(std::memory_order_acquire);
		if (First != TailCopy)
		{
			return AllocFromCache();
		}

		return ::new(std::malloc(sizeof(FNode))) FNode();
	}

private:
	alignas(std::hardware_destructive_interference_size) std::atomic<FNode*> Tail;
	FNode* Head;
	FNode* First;
	FNode* TailCopy;
};