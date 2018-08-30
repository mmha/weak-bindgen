#pragma once
#include "CXXCodeGen.hpp"
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Index/CodegenNameGenerator.h>

namespace weakbindgen {
	class FunctionDeclarationReplacerVisitor : public clang::RecursiveASTVisitor<FunctionDeclarationReplacerVisitor> {
		using Base = clang::RecursiveASTVisitor<FunctionDeclarationReplacerVisitor>;
		CXXCodeGen CCG;

		public:
		FunctionDeclarationReplacerVisitor(llvm::raw_ostream &out, clang::ASTContext &Context);
		bool VisitTranslationUnitDecl(clang::TranslationUnitDecl *);
		bool VisitFunctionDecl(clang::FunctionDecl *);
	};

	struct FunctionDeclarationReplacerConsumer : clang::ASTConsumer {
		FunctionDeclarationReplacerConsumer(llvm::StringRef InFile);
		void HandleTranslationUnit(clang::ASTContext &Context) override;

		private:
		void EmitModuleOpeningFunc();

		std ::string BindingImplSource;
		llvm::raw_string_ostream OS{BindingImplSource};
	};

	struct FunctionDeclarationReplacerAction : clang::ASTFrontendAction {
		std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler,
		                                                      llvm::StringRef InFile) override;
	};
}    // namespace weakbindgen
