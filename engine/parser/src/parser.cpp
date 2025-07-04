#include "parser.h"
#include "cursor.h"
#include <format>
#include <iostream>

Attribute ParseAttributes(const Cursor &cursor)
{
    const auto &attr = cursor.GetAnnotateAttr();
    if (!attr.empty())
    {
        if (attr == "reflect") return {true, false};
        else if (attr == "noreflect") return {false, true};
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
            if (node->children.empty() && node->fields.empty())
            {
                delete node;
            }
            else
            {
                parent->children.push_back(node);
            }
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

Node *Parser::ParseFile(const std::string &filename)
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
    }
    CXCursor rootCursor = clang_getTranslationUnitCursor(unit);
    Node *root = new Node;
    RecurseVisit(rootCursor, root);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return root;
}
