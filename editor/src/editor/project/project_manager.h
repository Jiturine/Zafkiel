#pragma once
#include "editor/project/project.h"

namespace Zafkiel 
{

class ProjectManager final 
{
  public:
    ProjectManager() = default;
    ~ProjectManager() = default;
    static void Init()
    {
        instance = new ProjectManager;
    }
    static void Destroy()
    {
        delete instance;
        instance = nullptr;
    }
    static ProjectManager &Instance() { return *instance; }
    
    static Ref<Project> CreateProject(const ProjectConfig &config) { return instance->CreateProjectImpl(config); }
    
    static Ref<Project> GetCurrentProject() { return instance->currentProject; }
    
  private:
    Ref<Project> CreateProjectImpl(const ProjectConfig &config);
    
    Ref<Project> currentProject;

    inline static ProjectManager *instance = nullptr;
};

}
