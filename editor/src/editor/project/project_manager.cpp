#include "project_manager.h"

namespace Zafkiel 
{

Ref<Project> ProjectManager::CreateProjectImpl(const ProjectConfig &config)
{
    currentProject = CreateRef<Project>(config);
    return currentProject;
}

}