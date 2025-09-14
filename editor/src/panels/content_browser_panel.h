#pragma once
#include "panels/panel.h"
#include "platform/filesystem/filesystem.h"

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