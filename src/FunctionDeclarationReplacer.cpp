#include "FunctionDeclarationReplacer.hpp"
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>

extern llvm::cl::opt<std::string> Output;
extern llvm::cl::list<std::string> ModuleFile;

weakbindgen::FunctionDeclarationReplacerVisitor::FunctionDeclarationReplacerVisitor(llvm::raw_ostream &out,
                                                                                    clang::ASTContext &Ctx)
    : CCG(out, Ctx) {
}

bool weakbindgen::FunctionDeclarationReplacerVisitor::VisitTranslationUnitDecl(clang::TranslationUnitDecl *) {
	const char name[] = "foo.h";
	llvm::SmallVector<llvm::StringRef, 1> list{name};
	CCG.GenerateIncludes(list);
	CCG.GenerateModuleInitializer(ModuleFile[0]);
	return true;
}

bool weakbindgen::FunctionDeclarationReplacerVisitor::VisitFunctionDecl(clang::FunctionDecl *Declaration) {
	auto const &Context = Declaration->getASTContext();
	auto const Location = Declaration->getLocation();
	if(!Context.getSourceManager().isInMainFile(Location)) {
		return true;
	}

	if(Declaration->isVariadic()) {
		auto &DE = Context.getDiagnostics();
		auto const ID = DE.getCustomDiagID(clang::DiagnosticsEngine::Error, "Vararg functions cannot be forwarded");
		Context.getDiagnostics().Report(Location, ID);
		return false;
	}

	if(auto const CMD = llvm::dyn_cast<clang::CXXMethodDecl>(Declaration)) {
		CCG.GenerateMemberFunctionCall(*CMD);
	} else {
		CCG.GenerateFreeFunctionCall(*Declaration);
	}

	return true;
}

weakbindgen::FunctionDeclarationReplacerConsumer::FunctionDeclarationReplacerConsumer(llvm::StringRef InFile) {
}

void weakbindgen::FunctionDeclarationReplacerConsumer::HandleTranslationUnit(clang::ASTContext &Context) {
	FunctionDeclarationReplacerVisitor Visitor{OS, Context};
	Visitor.TraverseDecl(Context.getTranslationUnitDecl());
	std::error_code EC;
	llvm::raw_fd_ostream out(Output.getValue(), EC, llvm::sys::fs::F_None);
	out << OS.str();
}

void weakbindgen::FunctionDeclarationReplacerConsumer::EmitModuleOpeningFunc() {
}

std::unique_ptr<clang::ASTConsumer>
    weakbindgen::FunctionDeclarationReplacerAction::CreateASTConsumer(clang::CompilerInstance &,
                                                                      llvm::StringRef InFile) {
	return std::make_unique<FunctionDeclarationReplacerConsumer>(InFile);
}
