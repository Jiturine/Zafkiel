#pragma once
#include "editor/Panels/Panel.h"
#include "Platform/Filesystem/Filesystem.h"

namespace Zafkiel
{
class ContentBrowserPanel : public Panel
{
  public:
    virtual void Render() override;
    void SetCurrentDirectory(const Path &dir);
  private:
    Path currentDirectory;
};
}