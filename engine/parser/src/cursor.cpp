#include "cursor.h"
#include "utils.h"

static std::string ToString(const CXString &str)
{
    auto cstr = clang_getCString(str);
    std::string res = cstr;
    clang_disposeString(str);
    return res;
}

Cursor::Cursor(const CXCursor &handle) : handle(handle) {}

CXCursorKind Cursor::GetKind() const
{
    return handle.kind;
}

CXSourceLocation Cursor::GetLocation() const
{
    return clang_getCursorLocation(handle);
}

std::string Cursor::GetAnnotateAttr() const
{
    std::string res{};

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
        auto str = static_cast<std::string *>(data);

        if (cursor.kind == CXCursor_AnnotateAttr)
        {
            *str = clang_getCString(clang_getCursorSpelling(cursor));
            return CXChildVisit_Break;
        }

        if (cursor.kind == CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        return CXChildVisit_Continue;
    };

    clang_visitChildren(handle, visitor, &res);

    return res;
}

std::string Cursor::GetSpelling() const
{
    return ToString(clang_getCursorSpelling(handle));
}

std::vector<Cursor> Cursor::GetChildren() const
{
    std::vector<Cursor> children;

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
        auto container = static_cast<std::vector<Cursor> *>(data);

        container->emplace_back(cursor);

        if (cursor.kind == CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        return CXChildVisit_Continue;
    };

    clang_visitChildren(handle, visitor, &children);

    return children;
}
