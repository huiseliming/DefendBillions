#pragma once
#include <type_traits>

template<typename T, typename TPtr = T*>
class TSingleton{
public:
    static_assert(std::is_same_v<std::decay_t<T>, std::decay_t<decltype(*std::declval<TPtr>())>>);
public:
    static TPtr GetStaticLocal();
    static TPtr This;
    static T& Self;
protected:
    TSingleton() {}
    TSingleton(const TSingleton&) = delete;
    TSingleton(TSingleton&&) = delete;
    TSingleton& operator=(const TSingleton&) = delete;
    TSingleton& operator=(TSingleton&&) = delete;
};

template<typename T, typename TPtr>
TPtr TSingleton<T, TPtr>::GetStaticLocal(){
    if constexpr (std::is_same_v<T*, TPtr>) {
        static TPtr _; 
        return _;
    } else {
        static TPtr _(new T()); 
        return _;
    }
}

template<typename T, typename TPtr>
TPtr TSingleton<T, TPtr>::This = TSingleton<T, TPtr>::GetStaticLocal();

template<typename T, typename TPtr>
T& TSingleton<T, TPtr>::Self = *This; 
