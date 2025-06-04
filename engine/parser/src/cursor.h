#pragma once

#include <vector>
#include <string>
#include "llvm_headers.h"

class Cursor
{
  public:
    Cursor(const Decl *);

    Decl::Kind GetKind() const;
    std::string GetSpelling() const;
    std::string GetDisplayName() const;
    std::vector<Cursor> GetChildren() const;
    const clang::AnnotateAttr *GetAnnotateAttr() const;
    SourceLocation GetLocation() const;

  private:
    const Decl *handle;
};