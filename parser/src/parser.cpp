#include "parser.h"
#include "cursor.h"
#include <format>
#include <iostream>
#include <string>

std::string GetCursorKindName(CXCursorKind kind)
{
    return clang_getCString(clang_getCursorKindSpelling(kind));
}

// void PrintCursorInfo(const Cursor &cursor, int depth = 0)
// {
//     std::string indent(depth * 2, ' ');
//     auto kind = cursor.GetKind();
//     auto spelling = cursor.GetSpelling();
//     auto attr = cursor.GetAnnotateAttr();

//     std::cout << indent << "Cursor: " << GetCursorKindName(kind) << std::endl
//               << " | Spelling: \"" << spelling << "\"" << std::endl
//               << " | Attribute: \"" << attr << "\"" << std::endl;

//     // Print location info
//     CXSourceLocation location = cursor.GetLocation();
//     CXFile file;
//     unsigned line, column, offset;
//     clang_getExpansionLocation(location, &file, &line, &column, &offset);
//     std::string filename = clang_getCString(clang_getFileName(file));

//     std::cout << " | Location: " << filename << ":" << line << ":" << column << std::endl;

//     // Print children
//     auto children = cursor.GetChildren();
//     for (const auto &child : children)
//     {
//         PrintCursorInfo(child, depth + 1);
//     }
// }

Attribute ParseAttributes(const Cursor &cursor)
{
    const auto &attr = cursor.GetAnnotateAttr();
    if (!attr.empty())
    {
        if (attr == "reflect" || attr == "refl") return {true, false};
        else if (attr == "noreflect" || attr == "norefl") return {false, true};
        else return {false, false};
    }
    return {false, false};
}

Attribute TransformAttributesByParent(Attribute node_attr, Attribute parent_attr)
{
    if (parent_attr.need_reflect)
    {
        if (node_attr.force_no_reflect) return {false, true};
        else return {true, false};
    }
    else if (parent_attr.force_no_reflect)
    {
        if (node_attr.need_reflect) return {true, false};
        else return {false, true};
    }
    else
    {
        return node_attr;
    }
}

void ParseEnumNode(const Cursor &cursor, EnumNode *node)
{
    for (const auto &child : cursor.GetChildren())
    {
        auto kind = child.GetKind();
        if (kind == CXCursor_EnumConstantDecl)
        {
            auto reflect_attr = ParseAttributes(cursor);
            reflect_attr = TransformAttributesByParent(reflect_attr, node->attr);
            if (reflect_attr.need_reflect)
            {
                node->items.push_back(child.GetSpelling());
            }
        }
    }
}

void Parser::RecurseVisit(const Cursor &cursor, Node *parent)
{
    CXSourceLocation loc = cursor.GetLocation();
    if (clang_Location_isInSystemHeader(loc))
    {
        return; // 跳过系统头文件
    }
    // PrintCursorInfo(cursor);
    auto kind = cursor.GetKind();
    if (kind == CXCursor_TranslationUnit)
    {
        for (const auto &child : cursor.GetChildren())
        {
            RecurseVisit(child, parent);
        }
    }
    if (kind == CXCursor_Namespace)
    {
        NamespaceNode *node = new NamespaceNode(cursor.GetSpelling());
        for (const auto &child : cursor.GetChildren())
        {
            RecurseVisit(child, node);
        }
        if (node->children.empty())
        {
            delete node;
        }
        else
        {
            parent->children.push_back(node);
        }
    }
    if (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl)
    {
        auto attr = ParseAttributes(cursor);
        attr = TransformAttributesByParent(attr, parent->attr);
        if (attr.need_reflect)
        {
            ClassNode *node = new ClassNode(cursor.GetSpelling());
            node->attr = attr;
            for (const auto &child : cursor.GetChildren())
            {
                RecurseVisit(child, node);
            }
            parent->children.push_back(node);
        }
    }
    if (kind == CXCursor_EnumDecl)
    {
        auto attr = ParseAttributes(cursor);
        attr = TransformAttributesByParent(attr, parent->attr);
        if (attr.need_reflect)
        {
            EnumNode *node = new EnumNode(cursor.GetSpelling());
            node->attr = attr;
            ParseEnumNode(cursor, node);
            if (node->items.empty())
            {
                delete node;
            }
            else
            {
                parent->children.push_back(node);
            }
        }
    }
    if (kind == CXCursor_FieldDecl)
    {
        if (parent->type == Node::Type::Class)
        {
            auto access_specifier = cursor.GetAccessSpecifier();
            if (access_specifier == CX_CXXPrivate)
            {
                return;
            }
            auto attr = ParseAttributes(cursor);
            attr = TransformAttributesByParent(attr, parent->attr);
            if (attr.need_reflect)
            {
                FieldNode *node = new FieldNode(cursor.GetSpelling());
                node->attr = attr;
                auto current_class = dynamic_cast<ClassNode *>(parent);
                current_class->fields.push_back(node);
            }
        }
    }
}

Node *Parser::ParseFile(const fs::path &filename)
{
    CXIndex index = clang_createIndex(0, 0);

    // 解析源文件
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        filename.c_str(),
        extraArgs.data(), extraArgs.size(),
        NULL, 0,
        CXTranslationUnit_None);
    if (!unit)
    {
        std::cout << "fail to parse the file!" << std::endl;
        clang_disposeIndex(index);
        return nullptr;
    }
    CXCursor rootCursor = clang_getTranslationUnitCursor(unit);

    // Debug: Print the AST structure
    std::cout << "Parsing file: " << filename << std::endl;

    Node *root = new Node;
    RecurseVisit(rootCursor, root);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return root;
}
