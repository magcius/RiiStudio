#include "root.hpp"
#include <filesystem>
#include <memory>
#include <vendor/llvm/Support/InitLLVM.h>

struct RootHolder {
  void create(int& argc, const char**& argv) {
    printf("Initializing LLVM\n");
    initLlvm = std::make_unique<llvm::InitLLVM>(argc, argv);

    window = std::make_unique<riistudio::frontend::RootWindow>();
  }

  void enter() { window->enter(); }

  riistudio::frontend::RootWindow& getRoot() {
    assert(window);
    return *window;
  }

private:
  std::unique_ptr<riistudio::frontend::RootWindow> window;
  std::unique_ptr<llvm::InitLLVM> initLlvm;
} sRootHolder;

int main(int argc, const char** argv) {
  if (argc > 0) {
    printf("%s\n", argv[0]);
    auto path = std::filesystem::path(argv[0]);
    if (path.is_absolute())
      std::filesystem::current_path(path.parent_path());
  }

  sRootHolder.create(argc, argv);

  if (argc > 1) {
    if (!strcmp(argv[1], "--update")) {
      sRootHolder.getRoot().setForceUpdate(true);
    } else {
      printf("File: %s\n", argv[1]);
      sRootHolder.getRoot().setCheckUpdate(false);
      sRootHolder.getRoot().openFile(argv[1]);
    }
  }

  sRootHolder.enter();

  return 0;
}
