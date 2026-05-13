#pragma once
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
  VimktorErr_t ChangeWindowRight();
  VimktorErr_t ChangeWindowLeft();


  
  VimktorErr_t ChangeWindowType(VimktorWindowType_t type);

  // colaboration
  VimktorErr_t InitCollab();
  VimktorErr_t EndCollab();
  VimktorErr_t ShowCollabUsers();
  VimktorErr_t ShowCollabSettings();
  CollabSession_t m_session;
  // variables

  VimktorMode_t m_mode;
  std::vector<std::unique_ptr<Window>> m_windows;
  std::vector<std::unique_ptr<Window>>::iterator m_current_window =
      m_windows.end();

private:
  static CommandList_t commandList;
  std::string m_filename;
};
