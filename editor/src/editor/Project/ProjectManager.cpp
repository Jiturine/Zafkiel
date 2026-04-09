#include "editor/Project/ProjectManager.h"

namespace Zafkiel 
{

Ref<Project> ProjectManager::CreateProjectImpl(const ProjectConfig &config)
{
    currentProject = CreateRef<Project>(config);
    return currentProject;
}

}