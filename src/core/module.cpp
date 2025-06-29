#include "module.h"

std::vector<ModulePtr>& IModule::GetRegisteredModules(){
    static std::vector<ModulePtr> modules;
    return modules;
}

std::vector<ModulePtr>& IModule::GetLoadedModules(){
    static std::vector<ModulePtr> modules;
    return modules;
}

bool IModule::StartupModules(){
    auto& registered_modules = GetRegisteredModules();
    int64_t registered_modules_size = registered_modules.size();
    for (int64_t i = 0; i < registered_modules_size; i++) {
        if (!registered_modules[i]->TryStartup()) {
            return false;
        }
    }
    return true;
    
}

void IModule::CleanupModules(){
    GetRegisteredModules().clear();
    std::vector<ModulePtr>& loaded_modules = GetLoadedModules();
    int64_t loaded_modules_size = loaded_modules.size();
    for (int64_t i = loaded_modules_size - 1; i >= 0; i--) {
        if (!loaded_modules[i]->m_ModuleStateFlags.HasAnyFlags(MSF_SkipCleanup))
        {
            loaded_modules[i]->Cleanup();
        }
    }
    GetLoadedModules().clear();
}

bool IModule::TryStartup() {
    bool r = false;
    if (!m_ModuleStateFlags.HasAnyFlags(MSF_ModuleLoading | MSF_ModuleLoaded)) {
        m_ModuleStateFlags.SetFlags(MSF_ModuleLoading);
        if (StartupDependencyModules()) {
            if (m_ModuleStateFlags.HasAnyFlags(MSF_SkipStartup) || Startup()) {
                r = true;
                m_ModuleStateFlags.SetFlags(MSF_ModuleLoaded);
                IModule::GetLoadedModules().push_back(shared_from_this());
            }
        }
        m_ModuleStateFlags.ClearFlags(MSF_ModuleLoading);
    }
    return r;
}
