#pragma once

template<typename ElementType>
struct TTypeCompatibleBytes
{
	using ElementTypeAlias_NatVisHelper = ElementType;
	ElementType* GetTypedPtr() { return (ElementType*)this; }
	const ElementType* GetTypedPtr() const { return (const ElementType*)this; }

	alignas(ElementType) uint8_t Pad[sizeof(ElementType)];
};
