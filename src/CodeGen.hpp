#pragma once
#include <clang/AST/DeclCXX.h>
#include <clang/Index/CodegenNameGenerator.h>
#include <llvm/ADT/StringRef.h>

namespace weakbindgen {
	template <typename T>
	struct Indent {
		T &Val;
		Indent(T &x)
		    : Val(x) {
			++Val;
		}
		~Indent() {
			--Val;
		}
	};

	template <typename T>
	struct Namespace {
		T &Stack;
		Namespace(T &x, llvm::StringRef str)
		    : Stack(x) {
			Stack.push_back(str);
		}
		~Namespace() {
			Stack.pop_back();
		}
	};

	enum class Init { Eager, Lazy };

	struct CodeGen {
		CodeGen(llvm::raw_ostream &OS, clang::ASTContext &Ctx)
		    : Context(Ctx)
		    , Out(OS) {
		}

		virtual ~CodeGen() = default;
		virtual void GenerateIncludes(llvm::SmallVectorImpl<llvm::StringRef> &Headers) = 0;
		virtual void GenerateModuleInitializer(llvm::StringRef ModuleName) = 0;
		virtual void GenerateFreeFunctionCall(clang::FunctionDecl &D) = 0;
		virtual void GenerateMemberFunctionCall(clang::CXXMethodDecl &D) = 0;

		protected:
		[[nodiscard]] llvm::raw_ostream &IOut() {
			return Out.indent(IndentationLevel);
		}

		[[nodiscard]] auto Indent() {
			return weakbindgen::Indent{IndentationLevel};
		}

		clang::ASTContext &Context;
		llvm::raw_ostream &Out;
		clang::index::CodegenNameGenerator Mangler{Context};

		private:
		unsigned IndentationLevel = 0u;
	};
}    // namespace weakbindgen
