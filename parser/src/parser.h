#pragma once

#include <filesystem>
#include "nodes.h"
#include "cursor.h"

namespace fs = std::filesystem;

class Parser
{
  public:
    Node *ParseFile(const fs::path &filename);

    Parser(std::vector<const char *> extraArgs) : extraArgs(extraArgs) {}

    std::vector<const char *> extraArgs;
  private:
    void RecurseVisit(const Cursor &cursor, Node *parent);
};
