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
#include <set>
#include <tuple>
#include <unordered_map>

auto compare_windows = [](const std::unique_ptr<Window> &p1,
                          const std::unique_ptr<Window> &p2) -> int {
  if (p2->GetWindowPosition().x != p1->GetWindowPosition().x) {
    return p2->GetWindowPosition().x < p1->GetWindowPosition().x;
  }

  return p2->GetWindowPosition().y < p1->GetWindowPosition().y;
};
typedef std::set<std::unique_ptr<Window>, decltype(compare_windows)>
    WindowSet_t;

class Vimktor {
public:
  typedef std::unordered_map<std::string, VimktorEvent_t> CommandList_t;

  Vimktor() : m_mode(NORMAL) {}
  Vimktor(const std::fstream &file) : m_mode(NORMAL) {}

  Vimktor(const std::string &fileName) {
    AddWindow(std::make_unique<ExploreWindow>());
  }

  void Init();
  void End();
  void Loop();

  VimktorErr_t InitCurses();
  VimktorErr_t HandleEvents();
  // private:
  VimktorErr_t AddWindow(std::unique_ptr<Window> &&win);
  VimktorErr_t CloseCurrentWindow();
  VimktorErr_t NewWindowVertical(std::unique_ptr<Window> &&win);
  VimktorErr_t NewWindowHorizontal(std::unique_ptr<Window> &&win); // renderer
  VimktorErr_t RenderWindow();

  // colaboration
  VimktorErr_t InitCollab();
  VimktorErr_t EndCollab();
  VimktorErr_t ShowCollabUsers();
  VimktorErr_t ShowCollabSettings();
  CollabSession_t m_session;
  // variables

  VimktorMode_t m_mode;
  WindowSet_t m_windows;
  WindowSet_t::iterator m_current_window = m_windows.end();

private:
  static CommandList_t commandList;
  std::string m_filename;
};
