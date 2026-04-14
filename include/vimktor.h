#include "colab.h"
#include "common.h"
#include "curses.h"
#include "input_manager.h"
#include "sequence.h"
#include "window.h"
#include <cstdint>
#include <expected>
#include <fstream>
#include <memory>
#include <tuple>
#include <unordered_map>

class Vimktor {
public:
  typedef std::unordered_map<std::string, VimktorEvent_t> CommandList_t;

  Vimktor() : m_mode(NORMAL) {}
  Vimktor( std::fstream &file)
      : m_mode(NORMAL) {}

  inline Vimktor(std::string &fileName) { LoadFile(fileName); }

  void Init();
  void End();
  void Loop();

  // private:

  // renderer
  VimktorErr_t RenderWindow();
  // file exploring

  // colaboration
  VimktorErr_t InitCollab();
  VimktorErr_t EndCollab();
  VimktorErr_t ShowCollabUsers();
  VimktorErr_t ShowCollabSettings();
  CollabSession_t m_session;
  // variables

  VimktorMode_t m_mode;
  std::vector<std::unique_ptr<Window>> m_windows;

private:
  static CommandList_t commandList;
  std::string m_filename;
};
