#pragma once

#include "llvm_headers.h"
#include "nodes.h"
#include "cursor.h"

class Parser
{
  public:
    Node *ParseFile(const std::string &filename);
    Parser(std::vector<std::string> extraArgs) : extraArgs(extraArgs) {}

  private:
    // 忽略所有错误
    class IgnoreDiagnosticConsumer : public clang::DiagnosticConsumer
    {
      public:
        void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel,
            const clang::Diagnostic &Info) override
        {
        }
    };
    class ParserASTConsumer : public ASTConsumer
    {
      public:
        void HandleTranslationUnit(ASTContext &Context) override;
        void RecurseVisit(const Cursor &cursor, Node *parent);
        static Node *root;
        static ASTContext *context;
    };
    class FrontendAction : public ASTFrontendAction
    {
      public:
        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override;
    };
    std::vector<std::string> extraArgs;
};
