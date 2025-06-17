#include "cursor.h"
#include "utils.h"

Cursor::Cursor(const Decl *handle) : handle(handle) {}

Decl::Kind Cursor::GetKind() const
{
    return handle->getKind();
}

const AnnotateAttr *Cursor::GetAnnotateAttr() const
{
    return handle->getAttr<AnnotateAttr>();
}

std::string Cursor::GetDisplayName(void) const
{
    if (const auto *namedDecl = dyn_cast<NamedDecl>(handle))
    {
        return namedDecl->getNameAsString();
    }
    return "";
}

SourceLocation Cursor::GetLocation() const
{
    return handle->getLocation();
}

std::vector<Cursor> Cursor::GetChildren() const
{
    std::vector<Cursor> children;
    if (auto *parent = dyn_cast<DeclContext>(handle))
    {
        for (Decl *child : parent->decls())
        {
            if (child->isImplicit()) continue;
            children.push_back(child);
        }
    }
    return children;
}
