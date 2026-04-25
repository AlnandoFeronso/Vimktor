#pragma once
#include <fstream>
#include <ios>
#include <string>
#include <string_view>
#define debugFile "debug.txt"

class Debug {
public:
    static void Log(std::string_view msg) {
    static Debug _instance;
    _instance.debugStream << msg << '\n';
    _instance.debugStream.flush();
  }

  std::fstream debugStream;

private:
  inline Debug() { debugStream.open(debugFile, std::ios_base::out); }
  inline ~Debug() { debugStream.close(); }
};
