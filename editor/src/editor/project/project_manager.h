#pragma once
#include "project.h"

namespace Zafkiel 
{

class ProjectManager final 
{
  public:
    ProjectManager() = default;
    ~ProjectManager() = default;
    static void Init()
    {
        instance.reset(new ProjectManager);
    }
    static void Destroy()
    {
        instance = nullptr;
    }
    static Scope<ProjectManager> &Instance() { return instance; }
    
    static Ref<Project> CreateProject(const ProjectConfig &config) { return instance->CreateProjectImpl(config); }
    
    static Ref<Project> GetCurrentProject() { return instance->currentProject; }
    
  private:
    Ref<Project> CreateProjectImpl(const ProjectConfig &config);
    
    Ref<Project> currentProject;

    inline static Scope<ProjectManager> instance;
};

}
