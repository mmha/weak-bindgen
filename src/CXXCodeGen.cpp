#include "CXXCodeGen.hpp"
#include <llvm/Support/Path.h>

namespace {
	void PrintType(llvm::raw_ostream &OS, clang::QualType QT, clang::ASTContext &Context) {
		clang::PrintingPolicy PP(Context.getLangOpts());
		PP.PolishForDeclaration = 1u;
		PP.FullyQualifiedName = 1u;

		auto const DifficultToDeclare = [](auto const Type) {
			return Type->isFunctionPointerType() || Type->isMemberPointerType() ||
			       Type->isMemberFunctionPointerType() || Type->isFunctionType();
		};
		bool WrapInTemplate = false;
		clang::QualType Type = QT;
		while(true) {
			if(DifficultToDeclare(Type)) {
				WrapInTemplate = true;
				break;
			}
			if(auto const PT = llvm::dyn_cast<clang::PointerType>(Type)) {
				Type = PT->getPointeeType();
			} else if(auto const RT = llvm::dyn_cast<clang::ReferenceType>(Type)) {
				llvm::outs() << "REference type\n";
				Type = RT->getPointeeType();
			} else {
				break;
			}
		}
		if(WrapInTemplate) {
			OS << "id<";
			QT.print(OS, PP);
			OS << ">";
		} else {
			QT.print(OS, PP);
		}
	}
}    // namespace

namespace weakbindgen {
	void CXXCodeGen::GenerateIncludes(llvm::SmallVectorImpl<llvm::StringRef> &Headers) {
		for(auto const H : Headers) {
			Out << "#include \"" << H << "\"\n";
		}
		Out << "#include <dlfcn.h>\n"
		       "#include <type_traits>\n\n"
		       "namespace weakbindgen {\n"
		       "    template<typname T> using id = T;\n"
		       "}\n"
		       "using namespace weakbindgen;\n\n";
	}

	void CXXCodeGen::GenerateModuleInitializer(llvm::StringRef ModuleName) {
		auto const LibName = llvm::sys::path::stem(ModuleName);
		// clang-format off
        Out << "namespace weakbindgen {\n"
              "static void *module_handle;\n"
              "bool const " << LibName << "_available = [] {\n"
              "    module_handle = ::dlopen(\"" << ModuleName << "\", RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);\n"
              "    return module_handle;\n"
              "}();\n"
              "}\n";
		// clang-format on
	}

	void CXXCodeGen::GenerateFreeFunctionCall(clang::FunctionDecl &Declaration) {
		GenerateFunctionSignature(Declaration);
		auto I = Indent();
		IOut() << "static auto const fptr = reinterpret_cast<decltype(" << Declaration.getNameAsString()
		       << ") *>(::dlsym(module_handle, \"" << Mangler.getName(&Declaration) << "\"));\n";
		IOut() << "return fptr(";
		if(Declaration.param_size() > 0) {
			Out << "p0";
			for(int i = 1; i < static_cast<int>(Declaration.param_size()); ++i) {
				Out << ", p" << i;
			}
		}
		Out << ");\n"
		       "}\n";
	}

	void CXXCodeGen::GenerateMemberFunctionCall(clang::CXXMethodDecl &Declaration) {
		GenerateFunctionSignature(Declaration);
		auto I = Indent();
		// clang-format off
        IOut() << "union hack {\n"
                "  decltype(&std::decay_t<decltype(*this)>::" << Declaration.getNameAsString()<< ") memfn;\n"
                "  void *fct;\n"
                "};\n"
                "static auto const fptr = [] {\n"
                "  hack res{};\n"
                "  res.fct = ::dlsym(module_handle, \"" << Mangler.getName(&Declaration) << "\");\n"
                "  return res.memfn;\n"
                "}();";
		// clang-format on
		IOut() << "return (this->*fptr)(";
		if(Declaration.param_size() > 0) {
			Out << "p0";
			for(int i = 1; i < static_cast<int>(Declaration.param_size()); ++i) {
				Out << ", p" << i;
			}
		}
		Out << ");\n"
		       "}\n";
	}

	void CXXCodeGen::GenerateFunctionSignature(clang::FunctionDecl &Declaration) {
		auto const &Type = *Declaration.getFunctionType();
		PrintType(Out, Type.getReturnType(), Context);
		Out << " ";
		Declaration.printQualifiedName(Out);
		Out << '(';

		if(Declaration.param_size() > 0) {
			int param_count = 0;
			auto param = Declaration.param_begin();
			for(; param < Declaration.param_end() - 1; ++param) {
				PrintType(Out, (*param)->getType(), Context);
				Out << " p" << param_count << ", ";
				++param_count;
			}
			PrintType(Out, (*param)->getType(), Context);
			Out << " p" << param_count;
		}
		Out << ") ";
		if(Type.isConst()) {
			Out << "const ";
		}
		if(Type.isVolatile()) {
			Out << "volatile ";
		}
		if(Type.isRestrict()) {
			Out << "__restrict ";
		}
		if(const auto FPT = llvm::dyn_cast<clang::FunctionProtoType>(&Type)) {
			switch(FPT->getRefQualifier()) {
				case clang::RQ_LValue:
					Out << "& ";
					break;
				case clang::RQ_RValue:
					Out << "&& ";
					break;
				case clang::RQ_None:
					break;
			}
#if LLVM_VERSION_MAJOR >= 7
			if(FPT->isNothrow()) {
				Out << "noexcept ";
			}
#else
			if(FPT->isNothrow(Context)) {
				Out << "noexcept ";
			}
#endif
		}
		Out << "{\n";
	}
}    // namespace weakbindgen
