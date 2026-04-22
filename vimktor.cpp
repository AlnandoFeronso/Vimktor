#include "include/vimktor.h"
#include "include/common.h"
#include "include/input_manager.h"
#include "include/sequence.h"
#include "include/vimktor_debug.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <curses.h>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

void Vimktor::Init() {
  InitCurses();
  int w, h;
  getmaxyx(stdscr, h, w);
  m_windows.push_back(std::make_unique<ExploreWindow>());
  m_current_window = m_windows.begin(); 
}

void Vimktor::End() { endwin(); }

VimktorErr_t Vimktor::InitCurses() {
  initscr();
  keypad(stdscr, TRUE);
  raw();
  nonl();
  set_escdelay(50);
  nodelay(stdscr, false);
  noecho();
  curs_set(1);
  init_color(COLOR, 0, 0, 0);
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::AddWindow(std::unique_ptr<Window> &&win) {
  if (m_windows.empty())
    m_windows.push_back(std::move(win));
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::RenderWindow() {
  if (m_current_window == m_windows.end() || m_windows.empty())
    return VimktorErr_t::MEMORY_ERROR;
  m_current_window->get()->Render();
  return VIMKTOR_OK;
}

// VimktorErr_t Vimktor::GetInput() {
//   VimktorEvent_t event = InputManager::Get().GetEvent(m_window, m_mode);
//   HandleEvents(event);
//   return VIMKTOR_OK;
// }

// VimktorErr_t Vimktor::HandleEvents(VimktorEvent_t event) {
//   VimktorErr_t err = VIMKTOR_OK;
//   switch (event) {
//
//   case EV_NONE:
//
//     break;
//   case EV_CURSOR_DOWN:
//     err = m_sequence.CursorMove(DOWN);
//     break;
//   case EV_CURSOR_UP:
//     err = m_sequence.CursorMove(UP);
//     break;
//   case EV_CURSOR_RIGHT:
//     err = m_sequence.CursorMove(RIGHT);
//     break;
//   case EV_CURSOR_LEFT:
//     err = m_sequence.CursorMove(LEFT);
//     break;
//   case EV_CLOSE:
//     m_mode = EXIT;
//     break;
//   case EV_ERASE_LINE:
//     m_sequence.EraseLineCursor();
//     break;
//   case EV_MODE_NORMAL:
//     m_mode = NORMAL;
//     break;
//   case EV_MODE_INSERT:
//     m_mode = INSERT;
//     break;
//   case EV_MODE_INSERT_RIGHT:
//     m_mode = INSERT;
//     m_sequence.m_mode = m_mode;
//     m_sequence.CursorMove(RIGHT);
//     break;
//   case EV_BACKSPACE:
//     m_sequence.EraseCharCursor();
//     break;
//   case EV_INSERT_TEXT: {
//     glyph_t gl = glyph_t(InputManager::Get().GetChar());
//     m_sequence.InsertCharCursor(gl);
//   } break;
//   case EV_GO_TO_SOL:
//     m_sequence.CursorMoveSol();
//     break;
//   case EV_GO_TO_NEXT_WORD:
//     m_sequence.CursorMoveWordNext();
//     break;
//   case EV_GO_TO_EOL:
//     m_sequence.CursorMoveEol();
//     break;
//   case EV_SAVE_FILE:
//     WriteFile();
//     HelperLog("saved to " + m_filename);
//     break;
//   case EV_NEW_LINE:
//     m_sequence.AddNewLineCursor();
//     break;
//   case EV_GET_COMMAND:
//     HandleCommands();
//     break;
//   case EV_FILE_EXPLORER:
//     WriteFile();
//     ExplorePath();
//     break;
//   case EV_ENTER_CURSOR_DIRECTORY:
//     OpenFileCursor();
//     break;
//   }
//   return VIMKTOR_OK;
// }
//
// VimktorErr_t Vimktor::HandleCommands() {
//   std::string cmd;
//
//   char16_t ch;
//
//   nodelay(m_window, 0);
//   while (1) {
//     HelperLog(":" + cmd);
//     wrefresh(m_window);
//     ch = wgetch(m_window);
//     if (ch == KEY_ESCAPE || ch == 13 || ch == KEY_ENTER) {
//       nodelay(m_window, 1);
//       // ch = wgetch(m_window);
//       break;
//       wrefresh(stdscr);
//     }
//     if (ch == KEY_BACKSPACE) {
//       if (cmd.size() > 0)
//         cmd.pop_back();
//
//     } else {
//       cmd.push_back(ch);
//     }
//   }
//   nodelay(m_window, 0);
//
//   // nodelay(m_window, 1);
//   HelperLog("                                           ");
//   wrefresh(stdscr);
//   if (commandList.contains(cmd)) {
//     HandleEvents(commandList[cmd]);
//   }
//
//   return VIMKTOR_OK;
// }
//
// VimktorErr_t Vimktor::OpenEmpty() {
//   LoadFile(".vimktor_temp");
//   return VIMKTOR_OK;
// }
//
// VimktorErr_t Vimktor::LoadFile(const std::string &fileName) {
//   if (fileName == ".") {
//     ExplorePath();
//     return VIMKTOR_OK;
//   }
//   std::fstream file;
//   m_filename = fileName;
//   file.open(fileName, std::ios::in);
//   if (!file.good()) {
//     file.close();
//     file.open(m_filename, std::ios::out | std::ios::in |
//     std::fstream::trunc); return FILE_ERROR;
//   }
//   Debug::Log("plik zapisu: " + m_filename);
//   m_sequence.LoadFile(file);
//   file.close();
//   return VIMKTOR_OK;
// };
//
// VimktorErr_t Vimktor::WriteFile(const std::string &fileName) {
//   std::fstream file;
//   file.open(fileName, std::ios::out);
//   if (!file.good()) {
//     return FILE_ERROR;
//   }
//   m_sequence.WriteFile(file);
//   file.close();
//   return VIMKTOR_OK;
// };
//
// VimktorErr_t Vimktor::WriteFile() {
//   std::fstream file;
//   file.open(m_filename, std::ios::out);
//   if (!file.good()) {
//     return FILE_ERROR;
//   }
//   m_sequence.WriteFile(file);
//   file.close();
//   return VIMKTOR_OK;
// };
//

VimktorErr_t Vimktor::HandleEvents() {

  auto ev = m_current_window->get()->HandleInput();
  if (ev == EV_NONE)
    return VIMKTOR_OK;
  switch (ev) {
  case EV_NEW_WINDOW_HORIZONTAL:
  default:
    return VIMKTOR_OK;
  }
}

void Vimktor::Loop() {
  while (m_mode != EXIT) {
    RenderWindow();
    HandleEvents();
  }
}
//
// position_t Vimktor::GetEditorDimensions() {
//   return position_t(getmaxx(m_window), getmaxy(m_window));
// }
//
// void Vimktor::HelperLog(const std::string &msg) {
//   position_t endPoint = GetEditorDimensions();
//   size_t y = endPoint.y - 1;
//   size_t x = 0;
//   wmove(m_window, y, x);
//   clrtoeol();
//   mvwprintw(m_window, y, x, "%s", msg.c_str());
// }
//
// Vimktor::CommandList_t Vimktor::commandList = {
//     {"w", EV_SAVE_FILE},
//     {"Explore", EV_FILE_EXPLORER},
//     {"q", EV_CLOSE},
//     {"Colab", EV_COLAB},
// };
//
//
// VimktorErr_t Vimktor::ExplorePath() {
//   m_sequence.m_cursorPos = position_t(0, 0);
//   m_mode = FILES;
//   HelperLog(std::filesystem::current_path().string());
//   m_sequence.LoadCurrentDirectory();
//   return VIMKTOR_OK;
// }
//
// VimktorErr_t Vimktor::ExplorePath(const std::string &path_str) {
//   m_sequence.m_cursorPos = position_t(0, 0);
//   m_mode = FILES;
//   std::filesystem::current_path(path_str);
//   HelperLog(std::filesystem::current_path().string());
//   m_sequence.LoadCurrentDirectory();
//   return VIMKTOR_OK;
// }
//
// VimktorErr_t Vimktor::OpenFileCursor() {
//   auto path = std::filesystem::current_path();
//
//   std::string path_str = path.string() + '/' + m_sequence.GetStringCursor();
//   if (std::filesystem::is_directory(path_str) ||
//       m_sequence.GetStringCursor() == "../" ||
//       m_sequence.GetStringCursor() == "./") {
//     ExplorePath(path_str);
//   } else {
//     LoadFile(m_sequence.GetStringCursor());
//     m_mode = NORMAL;
//   }
//   return VIMKTOR_OK;
// }
