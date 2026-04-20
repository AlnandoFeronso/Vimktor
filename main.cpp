#include "include/vimktor.h"
#include "include/vimktor_debug.h"
#include <expected>
#include <iostream>

int main(int argc, char **argv) {
  Vimktor app = Vimktor();
  if (argc > 1) {
    std::string fileName;
    fileName = argv[1];
    // app.LoadFile(fileName);
  }
  app.Init();
  app.Loop();
  app.End();
}
