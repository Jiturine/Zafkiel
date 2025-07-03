#pragma once

#include <vector>
#include <string>
#include <clang-c/Index.h>

class Cursor
{
  public:
    Cursor(const CXCursor &handle);

    CXCursorKind GetKind() const;
    std::string GetSpelling() const;
    std::vector<Cursor> GetChildren() const;
    CXSourceLocation GetLocation() const;
    std::string GetAnnotateAttr() const;

  private:
    CXCursor handle;
};