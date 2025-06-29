#pragma once
#include "forward.h"

#define ENUM_FLAGS_HELPER(EnumFlags)                                                                                                                                                                                                     \
FORCEINLINE           EnumFlags& operator|=(EnumFlags& p_LHS, EnumFlags p_RHS) { return p_LHS = static_cast<EnumFlags>(static_cast<std::underlying_type_t<EnumFlags>>(p_LHS) | static_cast<std::underlying_type_t<EnumFlags>>(p_RHS)); } \
FORCEINLINE           EnumFlags& operator&=(EnumFlags& p_LHS, EnumFlags p_RHS) { return p_LHS = static_cast<EnumFlags>(static_cast<std::underlying_type_t<EnumFlags>>(p_LHS) & static_cast<std::underlying_type_t<EnumFlags>>(p_RHS)); } \
FORCEINLINE           EnumFlags& operator^=(EnumFlags& p_LHS, EnumFlags p_RHS) { return p_LHS = static_cast<EnumFlags>(static_cast<std::underlying_type_t<EnumFlags>>(p_LHS) ^ static_cast<std::underlying_type_t<EnumFlags>>(p_RHS)); } \
FORCEINLINE constexpr EnumFlags  operator| (EnumFlags  p_LHS, EnumFlags p_RHS) { return         static_cast<EnumFlags>(static_cast<std::underlying_type_t<EnumFlags>>(p_LHS) | static_cast<std::underlying_type_t<EnumFlags>>(p_RHS)); } \
FORCEINLINE constexpr EnumFlags  operator& (EnumFlags  p_LHS, EnumFlags p_RHS) { return         static_cast<EnumFlags>(static_cast<std::underlying_type_t<EnumFlags>>(p_LHS) & static_cast<std::underlying_type_t<EnumFlags>>(p_RHS)); } \
FORCEINLINE constexpr EnumFlags  operator^ (EnumFlags  p_LHS, EnumFlags p_RHS) { return         static_cast<EnumFlags>(static_cast<std::underlying_type_t<EnumFlags>>(p_LHS) ^ static_cast<std::underlying_type_t<EnumFlags>>(p_RHS)); } \
FORCEINLINE constexpr bool       operator! (EnumFlags  p_Flags ) { return                        !static_cast<std::underlying_type_t<EnumFlags>>(p_Flags); }                                                                       \
FORCEINLINE constexpr EnumFlags  operator~ (EnumFlags  p_Flags ) { return static_cast<EnumFlags>(~static_cast<std::underlying_type_t<EnumFlags>>(p_Flags)); }                                                                      \

template <typename EnumFlags> constexpr typename std::underlying_type<EnumFlags>::type to_underlying(EnumFlags enum_flags) noexcept { return static_cast<typename std::underlying_type<EnumFlags>::type>(enum_flags); }

template<typename T>
struct TEnum {
    static_assert(std::is_enum_v<T>);
    using IntegerType = typename std::underlying_type<T>::type;

    FORCEINLINE constexpr auto GetFlags() -> T { return m_Value; }

    FORCEINLINE constexpr auto HasAnyFlags(IntegerType p_Flags) const -> IntegerType { return  (m_Value & p_Flags) != 0; }
    FORCEINLINE constexpr auto HasAllFlags(IntegerType p_Flags) const -> bool { return ((m_Value & p_Flags) == p_Flags); }

    FORCEINLINE auto SetFlags(IntegerType p_Flags = 0) -> void { m_Value |= p_Flags; }
    FORCEINLINE auto ClearFlags(IntegerType p_Flags = 0) -> void { m_Value &= ~p_Flags; }

    FORCEINLINE constexpr TEnum& operator|=(IntegerType p_RHS) { static_cast<T>(m_Value | p_RHS); return *this; }
    FORCEINLINE constexpr TEnum& operator&=(IntegerType p_RHS) { static_cast<T>(m_Value & p_RHS); return *this; }
    FORCEINLINE constexpr TEnum& operator^=(IntegerType p_RHS) { static_cast<T>(m_Value ^ p_RHS); return *this; }
    FORCEINLINE constexpr TEnum  operator| (IntegerType p_RHS) { static_cast<T>(m_Value | p_RHS); return *this; }
    FORCEINLINE constexpr TEnum  operator& (IntegerType p_RHS) { static_cast<T>(m_Value & p_RHS); return *this; }
    FORCEINLINE constexpr TEnum  operator^ (IntegerType p_RHS) { static_cast<T>(m_Value ^ p_RHS); return *this; }

    FORCEINLINE constexpr TEnum& operator|=(TEnum p_RHS) { static_cast<T>(m_Value | p_RHS.m_Value); return *this; }
    FORCEINLINE constexpr TEnum& operator&=(TEnum p_RHS) { static_cast<T>(m_Value & p_RHS.m_Value); return *this; }
    FORCEINLINE constexpr TEnum& operator^=(TEnum p_RHS) { static_cast<T>(m_Value ^ p_RHS.m_Value); return *this; }
    FORCEINLINE constexpr TEnum  operator| (TEnum p_RHS) { static_cast<T>(m_Value | p_RHS.m_Value); return *this; }
    FORCEINLINE constexpr TEnum  operator& (TEnum p_RHS) { static_cast<T>(m_Value & p_RHS.m_Value); return *this; }
    FORCEINLINE constexpr TEnum  operator^ (TEnum p_RHS) { static_cast<T>(m_Value ^ p_RHS.m_Value); return *this; }
    FORCEINLINE constexpr bool   operator! () { return !m_Value; }
    FORCEINLINE constexpr TEnum  operator~ () { ~m_Value; return *this; }

    constexpr operator bool() { return m_Value != 0; }
    constexpr operator T() { return T(m_Value); }
    constexpr operator IntegerType() { return m_Value; }

    IntegerType m_Value;
};
