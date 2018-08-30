#pragma once
#include "CodeGen.hpp"

namespace weakbindgen {
	struct CXXCodeGen final : CodeGen {
		CXXCodeGen(llvm::raw_ostream &OS, clang::ASTContext &Ctx)
		    : CodeGen(OS, Ctx) {
		}

		void GenerateIncludes(llvm::SmallVectorImpl<llvm::StringRef> &Headers) override;
		void GenerateModuleInitializer(llvm::StringRef ModuleName) override;
		void GenerateFreeFunctionCall(clang::FunctionDecl &D) override;
		void GenerateMemberFunctionCall(clang::CXXMethodDecl &D) override;

		private:
		void GenerateFunctionSignature(clang::FunctionDecl &D);
	};
}    // namespace weakbindgen
