#pragma once

#include <clang-c/Index.h>
#include "nodes.h"
#include "cursor.h"

class Parser
{
  public:
    Node *ParseFile(const std::string &filename);

    Parser(std::vector<const char *> extraArgs) : extraArgs(extraArgs) {}

    std::vector<const char *> extraArgs;
  private:
    void RecurseVisit(const Cursor &cursor, Node *parent);
};
