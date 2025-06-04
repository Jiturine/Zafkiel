#include "parser.h"
#include "cursor.h"
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

Attribute ParseAttributes(const Cursor &cursor)
{
    if (const auto *attr = cursor.GetAnnotateAttr())
    {
        const auto &spelling = attr->getAnnotation().str();
        if (spelling == "reflect") return {true, false};
        else if (spelling == "noreflect") return {false, true};
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
        if (kind == Decl::Kind::EnumConstant)
        {
            auto reflect_attr = ParseAttributes(cursor);
            reflect_attr = TransformAttributesByParent(reflect_attr, node->attr);
            if (reflect_attr.need_reflect)
            {
                node->items.push_back(child.GetDisplayName());
            }
        }
    }
}

void Parser::ParserASTConsumer::RecurseVisit(const Cursor &cursor, Node *parent)
{
    if (Parser::ParserASTConsumer::context->getSourceManager().isInSystemHeader(cursor.GetLocation()))
    {
        return;
    }
    auto kind = cursor.GetKind();
    if (kind == Decl::Kind::TranslationUnit)
    {
        for (const auto &child : cursor.GetChildren()) { RecurseVisit(child, parent); }
    }
    if (kind == Decl::Kind::Namespace)
    {
        NamespaceNode *node = new NamespaceNode(cursor.GetDisplayName());
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
    if (kind == Decl::Kind::CXXRecord)
    {
        auto attr = ParseAttributes(cursor);
        attr = TransformAttributesByParent(attr, parent->attr);
        if (attr.need_reflect)
        {
            ClassNode *node = new ClassNode(cursor.GetDisplayName());
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
    if (kind == Decl::Kind::Enum)
    {
        auto attr = ParseAttributes(cursor);
        attr = TransformAttributesByParent(attr, parent->attr);
        if (attr.need_reflect)
        {
            EnumNode *node = new EnumNode(cursor.GetDisplayName());
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
    if (kind == Decl::Kind::Field)
    {
        if (parent->type == Node::Type::Class)
        {
            auto attr = ParseAttributes(cursor);
            attr = TransformAttributesByParent(attr, parent->attr);
            if (attr.need_reflect)
            {
                FieldNode *node = new FieldNode(cursor.GetDisplayName());
                node->attr = attr;
                auto current_class = dynamic_cast<ClassNode *>(parent);
                node->is_member = true;
                current_class->fields.push_back(node);
            }
        }
    }
}

void Parser::ParserASTConsumer::HandleTranslationUnit(ASTContext &Context)
{
    context = &Context;
    const Cursor cursor(Context.getTranslationUnitDecl());
    root = new Node;
    RecurseVisit(cursor, root);
}

std::unique_ptr<ASTConsumer> Parser::FrontendAction::CreateASTConsumer(CompilerInstance &CI, StringRef file)
{
    CI.getFileManager().clearStatCache();
    // 配置诊断引擎忽略所有错误
    CI.getDiagnostics().setClient(new IgnoreDiagnosticConsumer, true);
    CI.getDiagnostics().setSuppressAllDiagnostics(true);
    CI.getDiagnostics().setIgnoreAllWarnings(true);
    return std::make_unique<ParserASTConsumer>();
}

Node *Parser::ParseFile(const std::string &filename)
{
    std::vector<std::string> ExtraArgs = {
        "-x", "c++",
        "-std=c++20",
        "-w", "-MG", "-M", "-DNDEBUG",
        "-D__clang__",
        "-ferror-limit=0", "-Wno-everything",
        "-Dreflect=clang::annotate(\"reflect\")",
        "-Dnoreflect=clang::annotate(\"noreflect\")",
        "-I/usr/include/c++/13"};
    // 读取源文件内容
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> FileOrErr = llvm::MemoryBuffer::getFile(filename);
    if (std::error_code EC = FileOrErr.getError())
    {
        llvm::errs() << "Error reading file: " << EC.message() << "\n";
        return nullptr;
    }
    // 运行工具
    tooling::runToolOnCodeWithArgs(std::make_unique<FrontendAction>(), (*FileOrErr)->getBuffer(), ExtraArgs, filename.c_str());
    return ParserASTConsumer::root;
}

Node *Parser::ParserASTConsumer::root = nullptr;
ASTContext *Parser::ParserASTConsumer::context = nullptr;