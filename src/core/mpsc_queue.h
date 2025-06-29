// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <atomic>
#include <optional>
#include <cstdlib>
#include "template.h"

/** 
 * Fast multi-producer/single-consumer unbounded concurrent queue.
 * Based on http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue
 */
template<typename T>
class TMpscQueue final
{
	using ElementType = T;

	void DestructElement(ElementType* Element) {
		if constexpr (!std::is_trivially_destructible_v<ElementType>)
		{
			Element->ElementType::~ElementType();
		}
	}

public:

	TMpscQueue(const TMpscQueue&) = delete;
	TMpscQueue(TMpscQueue&&) = delete;
	const TMpscQueue& operator=(const TMpscQueue&) = delete;
	const TMpscQueue& operator=(TMpscQueue&&) = delete;

	TMpscQueue()
	{
		FNode* Sentinel = ::new(std::malloc(sizeof(FNode))) FNode;
		Head.store(Sentinel, std::memory_order_relaxed);
		Tail = Sentinel;
	}

	~TMpscQueue()
	{
		FNode* Next = Tail->Next.load(std::memory_order_relaxed);

		// sentinel's value is already destroyed
		std::free(Tail);

		while (Next != nullptr)
		{
			Tail = Next;
			Next = Tail->Next.load(std::memory_order_relaxed);

			DestructElement((ElementType*)&Tail->Value);
			std::free(Tail);
		}
	}

	template <typename... ArgTypes>
	void Enqueue(ArgTypes&&... Args)
	{
		FNode* New = ::new(std::malloc(sizeof(FNode))) FNode;
		::new ((void*)&New->Value) ElementType(std::forward<ArgTypes>(Args)...);

		FNode* Prev = Head.exchange(New, std::memory_order_acq_rel);
		Prev->Next.store(New, std::memory_order_release);
	}

	std::optional<ElementType> Dequeue()
	{
		FNode* Next = Tail->Next.load(std::memory_order_acquire);

		if (Next == nullptr)
		{
			return {};
		}

		ElementType* ValuePtr = (ElementType*)&Next->Value;
		std::optional<ElementType> Res{ std::move(*ValuePtr) };
		DestructElement(ValuePtr);

		std::free(Tail); // current sentinel

		Tail = Next; // new sentinel
		return Res;
	}

	bool Dequeue(ElementType& OutElem)
	{
		std::optional<ElementType> LocalElement = Dequeue();
		if (LocalElement.has_value())
		{
			OutElem = std::move(LocalElement.value());
			return true;
		}

		return false;
	}

	// as there can be only one consumer, a consumer can safely "peek" the tail of the queue.
	// returns a pointer to the tail if the queue is not empty, nullptr otherwise
	// there's no overload with TOptional as it doesn't support references
	ElementType* Peek() const
	{
		FNode* Next = Tail->Next.load(std::memory_order_acquire);

		if (Next == nullptr)
		{
			return nullptr;
		}

		return (ElementType*)&Next->Value;
	}

	bool IsEmpty() const
	{
		return Tail->Next.load(std::memory_order_acquire) == nullptr;
	}

private:
//#pragma warning(push)
//#pragma warning(disable: 4324)
//	struct alignas(std::hardware_destructive_interference_size) FNode
//#pragma warning(pop)
	struct FNode
	{
		std::atomic<FNode*> Next{ nullptr };
		TTypeCompatibleBytes<ElementType> Value;
	};

private:
	alignas(std::hardware_destructive_interference_size) std::atomic<FNode*> Head; // accessed only by producers
	FNode* Tail; // accessed only by consumer, hence should be on a different cache line than `Head`
};