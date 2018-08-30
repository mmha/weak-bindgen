#include "FunctionDeclarationReplacer.hpp"
#include "include_paths.hpp"
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

using namespace clang::tooling;
using namespace llvm;

llvm::cl::OptionCategory WeakBindGenCategory("weak-bindgen options");
cl::opt<bool> LazyInitOption("lazy",
                             cl::desc("Lazily resolve symbols instead of loading everything in an init function"));
cl::list<std::string> ModuleFile("l", cl::desc("shared library to generate weak bindings for"), cl::OneOrMore);
cl::opt<std::string> Output("o", cl::Required);
cl::opt<bool> VerifyMode("verify");
cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

void add_system_include_dirs(ClangTool &Tool) noexcept {
	ArgumentsAdjuster ardj1 = getInsertArgumentAdjuster(weakbindgen::clang_internal_include_path);
	ArgumentsAdjuster ardj2 = getInsertArgumentAdjuster(weakbindgen::clang_include_path);
	Tool.appendArgumentsAdjuster(ardj1);
	Tool.appendArgumentsAdjuster(ardj2);
}

int main(int argc, const char **argv) {
	LazyInitOption.setCategory(WeakBindGenCategory);
	CommonOptionsParser OptionsParser(argc, argv, WeakBindGenCategory);
	ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

	add_system_include_dirs(Tool);

	return Tool.run(newFrontendActionFactory<weakbindgen::FunctionDeclarationReplacerAction>().get());
}
