#pragma once
#include "enum.h"

#define DECL_MODULE(T) \
public:                      \
    static T* This;          \
    static T& Self;          \

#define IMPL_MODULE(T)      \
T* T::This = T::GetStaticLocal();  \
T& T::Self = *T::GetStaticLocal(); \

enum ModuleStateFlags : uint64_t {
    MSF_ModuleLoaded     = 1ui64 << 0,
    MSF_ModuleLoading    = 1ui64 << 1,
    MSF_ModuleLoadFailed = 1ui64 << 2,
    MSF_SkipStartup      = 1ui64 << 3,
    MSF_SkipCleanup      = 1ui64 << 4,

    // -------custom-----------
    //       32 -- 47
    // ------------------------
    MSF_HotReloadable = 1ui64 << 63,
};

ENUM_FLAGS_HELPER(ModuleStateFlags);

using ModulePtr = std::shared_ptr<class IModule>;
template<typename T>
using TModulePtr = std::shared_ptr<T>;

class EXPORT_MODULE_API IModule : public std::enable_shared_from_this<IModule> {
protected:
    static std::vector<ModulePtr>& GetRegisteredModules();
    static std::vector<ModulePtr>& GetLoadedModules();
public:
    static bool StartupModules();
    static void CleanupModules();

public:
    virtual ~IModule() {}
    virtual bool Startup() { return true; }
    virtual void Cleanup() { }

protected:
    virtual bool StartupDependencyModules() = 0;

    bool TryStartup();
    friend class MainLoop;

protected:
    std::string m_Name;
    TEnum<ModuleStateFlags> m_ModuleStateFlags{0};
    std::vector<ModulePtr> m_DependencyModules;
};

template <typename T, typename ... DependencyModules>
class TModule : public IModule
{
    bool ForEachStartupDependencyModule() { return true; }

    template <typename DependencyModule>
    bool ForEachStartupDependencyModule() {
        auto& dependency_module = DependencyModule::GetStaticLocal();
        return dependency_module->TryStartup();
    }

    template <typename DependencyModule, typename ... Args>
    bool ForEachStartupDependencyModule() {
        auto& dependency_module = DependencyModule::GetStaticLocal();
        if (!dependency_module->TryStartup()) return false;
        return ForEachStartupDependencyModule<Args...>();
    }

    virtual bool StartupDependencyModules() override{
        return ForEachStartupDependencyModule();
    }

public:
    static T* GetStaticLocal();
public:
    TModule() : IModule() {
    }

protected:
    TModule(const TModule&) = delete;
    TModule(TModule&&) = delete;
    TModule& operator=(const TModule&) = delete;
    TModule& operator=(TModule&&) = delete;

};

template<typename T, typename ... DependencyModules>
T* TModule<T, DependencyModules...>::GetStaticLocal() {
    static T* ptr = [] {
        static TModulePtr<T> module_ptr = std::make_shared<T>();
        IModule::GetRegisteredModules().push_back(module_ptr);
        return module_ptr.get();
    }();
    return ptr;
}



